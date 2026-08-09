#include <stdio.h>
#include "structs.h"
#include "config.h"
#include <dirent.h>
#include <taglib/tag_c.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

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