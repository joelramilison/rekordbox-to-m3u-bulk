#include "parser.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "config.h"
#include <string.h>
#include "structs.h"

// Recursively go through all nodes which are either playlist folders or playlists.
void findPlaylists(xmlNodePtr cur, struct PlaylistNode* plNodePtr) {

	// Parse playlist/folder name
	xmlChar* name = xmlGetProp(cur, (const xmlChar *)"Name");
	if (name == NULL) {
		fprintf(stderr, "XML Parse error: Couldn't parse PlaylistNode name.\n");
		exit(1);
	}
	strlcpy(plNodePtr->name, (char *) name, MAXIMUM_PLAYLIST_NODE_NAME_LENGTH + 1);

	// Parse type value (playlist vs. folder)
	xmlChar* typeString = xmlGetProp(cur, (const xmlChar*) "Type");
	char* endPtr = NULL;
	long int type = strtol((char *)typeString, &endPtr, 0);
	if (endPtr == (char *) typeString) {
		fprintf(stderr, "XML Parse error: Couldn't parse PlaylistNode type at node '%s'.\n", (char *) name);
		exit(1);
	}
	if (type == 1) {
		plNodePtr->isPlaylist = 1;
	} else if (type != 0) {
		fprintf(stderr, "XML Parse error: Parsed bad type value for PlaylistNode '%s'.\n", (char *) name);
		exit(1);
	}
	
	// Parse count value (number of tracks or children folders)
	xmlChar* countString = xmlGetProp(
		cur, (const xmlChar*) type ? (const xmlChar * )"Entries" : (const xmlChar * )"Count");
	endPtr = NULL;
	long int count = strtol((char *) countString, &endPtr, 0);
	if (endPtr == (char *) countString) {
		fprintf(stderr, "XML Parse error: Couldn't parse PlaylistNode count value at node '%s'.\n", (char *) name);
		exit(1);
	}
	// If value is too big for size_t or if value is negative, exit
	if ((LONG_MAX < SIZE_MAX || count < (long int) SIZE_MAX) && count >= 0) {
		plNodePtr->count = (size_t) count;
	} else {
		fprintf(stderr, "XML Parse error: Parsed bad count value for PlaylistNode '%s'.\n", (char *) name);
		exit(1);
	}

	// Handle playlist: Import it
	if (type == 1) {
		importPlaylist(cur, plNodePtr);
		return;

	}

	// Handle folder: Cycle through children
	plNodePtr->childrenNodes = malloc(plNodePtr->count * sizeof(struct PlaylistNode));
	cur = cur->xmlChildrenNode;
	int nodesFound = 0;
	while (cur != NULL) {
		if (strcmp((char*) cur->name, "NODE") == 0) {
			findPlaylists(cur, &(plNodePtr->childrenNodes[nodesFound]));
			nodesFound += 1;
		}
		cur = cur->next;
	}
	if (nodesFound != count) {
		fprintf(stderr, "XML Parse error: Found %d children nodes under PlaylistNode '%s' but expected %d.\n",
			nodesFound, (char *) name, (int)count);
		exit(1);
	}
}

void importPlaylist(xmlNodePtr cur, struct PlaylistNode* plNodePtr) {

	plNodePtr->trackIds = malloc(plNodePtr->count * sizeof(char *));

	cur = cur->xmlChildrenNode;
	int tracksFound = 0;
	while (cur != NULL) {
		if (strcmp((char*) cur->name, "TRACK") == 0) {
			xmlChar* trackId = xmlGetProp(cur, (const xmlChar *)"Key");
			if(trackId == NULL || strcmp((char *) trackId, "") == 0) {
				fprintf(stderr, "XML Parse error: Couldn't parse track ID for track in playlist '%s'.\n", plNodePtr->name);
				exit(1);
			}
			plNodePtr->trackIds[tracksFound] = malloc(strlen((char *) trackId));
			strlcpy((char *) trackId, plNodePtr->trackIds[tracksFound], MAXIMUM_TRACK_ID_LENGTH + 1);
			tracksFound += 1; 
		}
		cur = cur->next;
	}

	if (tracksFound != plNodePtr->count) {
		fprintf(stderr, "XML Parse error: Found %d tracks under playlist '%s' but expected %d.\n",
			tracksFound, plNodePtr->name, (int) (plNodePtr->count));
		exit(1);
	}
}

void parseCollectionTracks(struct CollectionTracksArray* collectionTracksArrayPtr, xmlNodePtr cur) {

	int tracksSize = 1000;
	collectionTracksArrayPtr->array = malloc(tracksSize * sizeof(struct CollectionTrack));
	struct CollectionTrack* collectionTracks = collectionTracksArrayPtr->array;
	memset(collectionTracks, 0, tracksSize * sizeof(struct CollectionTrack));
	
	int counter = 0;
	while(cur != NULL) {
		// Only handle TRACK nodes, not 'text' nodes
		if (strcmp((char *)cur->name, "TRACK") != 0) {
			cur = cur->next;
			continue;
		}
		counter += 1;
		if (counter > tracksSize) {
			if(!realloc(collectionTracks, tracksSize * 2 * sizeof(struct CollectionTrack))) {
				printf("Error reallocating memory.\n");
				exit(1);
			}
			memset(collectionTracks + tracksSize, 0, tracksSize * sizeof(struct CollectionTrack));
			tracksSize = tracksSize * 2;
		}
		xmlChar* trackId = xmlGetProp(cur, (const xmlChar*)"TrackID");
		xmlChar* title = xmlGetProp(cur, (const xmlChar*)"Name");
		xmlChar* artist = xmlGetProp(cur, (const xmlChar*)"Artist");
		if(strcmp((char *) artist, "rekordbox") == 0) {
			counter -= 1;
			cur = cur->next;
			continue;
		}
		if (strlen((char* ) trackId) > MAXIMUM_TRACK_ID_LENGTH) {
			fprintf(stderr, "Error: Track ID longer than maximum allowed length.\nTitle: %s\nArtist:%s\n",
				title, artist);
			exit(1);
		}
		strlcpy(collectionTracks[counter-1].trackId, (char *)trackId, MAXIMUM_TRACK_ID_LENGTH + 1);
		strlcpy(collectionTracks[counter-1].title, (char *)title, MAXIMUM_TITLE_LENGTH + 1);
		strlcpy(collectionTracks[counter-1].artist, (char *)artist, MAXIMUM_ARTIST_LENGTH + 1);
		xmlFree(trackId);
		xmlFree(title);
		xmlFree(artist);
		cur = cur->next;
	}
	collectionTracksArrayPtr->tracksCount = counter;

}
void parseXml(struct CollectionTracksArray* collectionTracksArrayPtr, struct PlaylistNode* plNodePtr) {

	xmlDocPtr doc = xmlParseFile(REKORDBOX_COLLECTION_XML_PATH);
	if (doc == NULL) {
		fprintf(stderr, "XML Parse error: Error parsing XML file.\n");
		exit(1);
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr, "XML Parse error: Empty document.\n");
		exit(1);
	}

	// Go to COLLECTION node
	cur = cur->xmlChildrenNode;
	while(cur != NULL && (xmlStrcmp(cur->name, (const xmlChar*)"COLLECTION")) != 0) {
		cur = cur->next;
	}
	if (cur == NULL) {
		fprintf(stderr, "XML Parse error: Couldn't find COLLECTION node.\n");
	}
	// Saving this pointer for the collection parsing later
	xmlNodePtr collectionNodePtr = cur;
	
	// Go to PLAYLISTS node
	while(cur != NULL && (xmlStrcmp(cur->name, (const xmlChar*)"PLAYLISTS")) != 0) {
		cur = cur->next;
	}
	if (cur == NULL) {
		fprintf(stderr, "XML Parse error: Couldn't find PLAYLISTS node.\n");
	}

	// Go to root playlists NODE
	cur = cur->xmlChildrenNode;
	while(cur != NULL && (xmlStrcmp(cur->name, (const xmlChar*)"NODE")) != 0) {
		cur = cur->next;
	}
	if (cur == NULL) {
		fprintf(stderr, "XML Parse error: Couldn't find root playlists NODE node.\n");
	}

	// Parse the playlists and populate the PlaylistNode
	findPlaylists(cur, plNodePtr);
	
	// Parse the collection of tracks themselves
	parseCollectionTracks(collectionTracksArrayPtr, collectionNodePtr->xmlChildrenNode);
	
	xmlFreeDoc(doc);
}