#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <taglib/tag_c.h>

const char* REKORDBOX_COLLECTION_XML_PATH = "/Users/joelramilison/Documents/Rekordbox_Collection.xml";
// Directory to recursively look for all .mp3 and .flac files
const char* LOCAL_FILES_ROOT_DIRECTORY = "/Users/joelramilison/Cloud Option DJ team Dropbox/Joel Ramilison/rekordbox/contents_1076899183";
const size_t MAXIMUM_TITLE_LENGTH = 150;
const size_t MAXIMUM_ARTIST_LENGTH = 150;
const size_t MAXIMUM_TRACK_ID_LENGTH = 20;
const size_t MAXIMUM_PATH_LENGTH = 1024;

struct CollectionTrack {
	
	char title[MAXIMUM_TITLE_LENGTH + 1];
	char artist[MAXIMUM_ARTIST_LENGTH + 1];
	char path[MAXIMUM_PATH_LENGTH + 1];
	char trackId[MAXIMUM_TRACK_ID_LENGTH + 1];
};

struct LocalTrack {

	char title[MAXIMUM_TITLE_LENGTH + 1];
	char artist[MAXIMUM_ARTIST_LENGTH + 1];
	char path[MAXIMUM_PATH_LENGTH + 1];
};

struct LocalTracksArray {

	struct LocalTrack* array;
	size_t tracksCount;
	size_t allocatedBytes;
};

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

// Create full path out of first and second, adding or removing '/' in between them as needed.
// Pass a string pointer to populate.
char* concatPath(char* fullPath, const char* first, const char* second, size_t maxLen) {

	if(*first != '/') {
		fprintf(stderr, "String 1 has to begin with '/' to get a full path.");
		exit(1);
	}

	int counter = 0;
	while (*first) {
		fullPath[counter] = *first;
		first += 1;
		counter += 1;
		if (counter > maxLen) {
			fprintf(stderr, "Strings too long for the maximum path length.");
			exit(1);
		}
	}

	// Add or remove one '/' in between if needed
	if (*(first - 1) != '/' && *second != '/') {
		fullPath[counter] = '/';
		counter += 1;
	} else if (*(first - 1) == '/' && *second == '/') {
		second += 1;
	}

	while (*second) {
		fullPath[counter] = *second;
		second += 1;
		counter += 1;
		if (counter > maxLen) {
			fprintf(stderr, "Strings too long for the maximum path length.");
			exit(1);
		}
	}
	fullPath[counter] = '\0';
	return fullPath;
}

void addLocalTrack(struct LocalTracksArray* localTracksArray, char* path, char* title, char* artist) {

	// Check if needs more memory when track found
	if (localTracksArray->allocatedBytes < localTracksArray->tracksCount * sizeof(struct LocalTrack)) {
		localTracksArray->allocatedBytes = localTracksArray->allocatedBytes * 2;
		localTracksArray->array = realloc(localTracksArray->array, localTracksArray->allocatedBytes);
	}
	strcpy(localTracksArray->array[localTracksArray->tracksCount].title, title);
	strcpy(localTracksArray->array[localTracksArray->tracksCount].artist, artist);
	strcpy(localTracksArray->array[localTracksArray->tracksCount].path, path);
	localTracksArray->tracksCount += 1;
	
}
void recursiveTrackSearch(char *startDir, struct LocalTracksArray* localTracksArray) {

	DIR* dirPtr = opendir(startDir);
	if (dirPtr == NULL) {
		// TODO: Implement errno
		fprintf(stderr, "Error opening directory: %s.\n", startDir);
		exit(1);
	}
	
	struct dirent* dirEntry;
	int tracksCount = 0;
	// Check next entry in directory

	while ((dirEntry = readdir(dirPtr)) != NULL) {

		char* name = dirEntry->d_name;

		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
			continue;
		}
		// Check if file or directory
		struct stat statBuf;
		char* fullPath = malloc((MAXIMUM_PATH_LENGTH + 1) * sizeof(char));
		concatPath(fullPath, startDir, name, MAXIMUM_PATH_LENGTH);
		if (stat(fullPath, &statBuf) != 0) {
			fprintf(stderr, "Error getting status for path: %s\nError message: %s\n", fullPath, strerror(errno));
			exit(1);
		}	

		// If found a directory
		if ((statBuf.st_mode & S_IFMT) == S_IFDIR) {
			recursiveTrackSearch(fullPath, localTracksArray);
			continue;
		}
		// If found a file
		if ((statBuf.st_mode & S_IFMT) == S_IFREG) {

			size_t nameLen = strlen(name);
			// If MP3 or FLAC
			if ((nameLen > 4 && (strcmp(name + (nameLen - 4), ".mp3") == 0))
				|| (nameLen > 5 && (strcmp(name + (nameLen - 5), ".flac")) == 0)) {

				TagLib_File* tagLibFile = taglib_file_new(fullPath);
				if (!taglib_file_is_valid(tagLibFile)) {
					fprintf(stderr, "Error: TagLib: Couldn't read tags from file: %s\n", fullPath);
					exit(1);
				}
				TagLib_Tag* tagLibTag = taglib_file_tag(tagLibFile);
				char* title = taglib_tag_title(tagLibTag);
				char* artist = taglib_tag_artist(tagLibTag);
				addLocalTrack(localTracksArray, fullPath, title, artist);
				taglib_file_free(tagLibFile);
			} 
		}
	}
	closedir(dirPtr);
}

int main(void) {

	// Load in collection tracks
	struct CollectionTrack* collectionTracks;
	int collectionTracksCount = parseXml(collectionTracks);	

	struct LocalTracksArray localTracksArray = {0};
	localTracksArray.allocatedBytes = 1000 * sizeof(struct LocalTrack);
	localTracksArray.array = malloc(localTracksArray.allocatedBytes);
	recursiveTrackSearch((char* ) LOCAL_FILES_ROOT_DIRECTORY, &localTracksArray);
	return 0;
}
