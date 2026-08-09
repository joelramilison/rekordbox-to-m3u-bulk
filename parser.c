#include <libxml/parser.h>
#include <libxml/tree.h>
#include "config.h"
#include <string.h>
#include "structs.h"

// TODO: Parse playlists into an array or maybe a nodes structure (because of subfolders) if it's in xml
int parseXml(struct CollectionTrack* collectionTracks) {

	int tracksSize = 1000;
	collectionTracks = malloc(tracksSize * sizeof(struct CollectionTrack));
	memset(collectionTracks, 0, tracksSize * sizeof(struct CollectionTrack));

	xmlDocPtr doc = xmlParseFile(REKORDBOX_COLLECTION_XML_PATH);
	if (doc == NULL) {
		fprintf(stderr, "Error parsing XML file.\n");
		exit(1);
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr, "Empty document.\n");
		exit(1);
	}

	// Go to COLLECTION node
	cur = cur->xmlChildrenNode;
	while(cur != NULL && (xmlStrcmp(cur->name, (const xmlChar*)"COLLECTION")) != 0) {
		cur = cur->next;
	}
	if (cur == NULL) {
		fprintf(stderr, "Couldn't find COLLECTION node.\n");
	}
	
	// Cycle through TRACK nodes and get track info
	cur = cur->xmlChildrenNode;
	int counter = 0;
	while(cur != NULL) {
		// Only handle TRACK nodes, not 'text' nodes
		if (strcmp((char *)cur->name, "TRACK") != 0) {
			cur = cur->next;
			continue;
		}
		counter += 1;
		if (counter > tracksSize) {
			tracksSize = tracksSize * 2;
			if(!realloc(collectionTracks, tracksSize * sizeof(struct CollectionTrack))) {
				printf("Error reallocating memory.\n");
				exit(1);
			}
		}
		xmlChar* trackId = xmlGetProp(cur, (const xmlChar*)"TrackID");
		xmlChar* title = xmlGetProp(cur, (const xmlChar*)"Name");
		xmlChar* artist = xmlGetProp(cur, (const xmlChar*)"Artist");
		strlcpy(collectionTracks[counter-1].trackId, (char *)trackId, MAXIMUM_TRACK_ID_LENGTH + 1);
		strlcpy(collectionTracks[counter-1].title, (char *)title, MAXIMUM_TITLE_LENGTH + 1);
		strlcpy(collectionTracks[counter-1].artist, (char *)artist, MAXIMUM_ARTIST_LENGTH + 1);
		xmlFree(trackId);
		xmlFree(title);
		xmlFree(artist);
		cur = cur->next;
	}

	xmlFreeDoc(doc);
	return counter;

}