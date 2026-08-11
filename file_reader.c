#include "file_reader.h"
#include <stdio.h>
#include "structs.h"
#include "config.h"
#include <dirent.h>
#include <taglib/tag_c.h>
#include <string.h>
#include <errno.h>
#include "file_io_general.h"

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

// dest needs 6 bytes. Returns 1 if found file extension.
void getFileExtension(char* dest, char* fileName) {

	int len = strlen(fileName);
	int dotIndex = -1;
	for (int i = len - 1; i >= 0; i--) {
		if (fileName[i] == '.') {
			dotIndex = i;
			break;
		}
	}

	// If no dot found
	if (dotIndex == -1) {
		dest[0] = '\0';
		return;
	}

	// Ignore if file extension is longer than 5 or fileName ends with '.'
	int lastExtensionIndex = len - 1;
	int firstExtensionIndex = dotIndex + 1;
	int extensionLen = lastExtensionIndex - firstExtensionIndex + 1;
	if (extensionLen > 5 || lastExtensionIndex == dotIndex) {
		dest[0] = '\0';
		return;
	}

	for (int i = firstExtensionIndex; i <= lastExtensionIndex; i++) {
		*dest = fileName[i];
		dest++;
	}
	*dest = '\0';
}

void recursiveTrackSearch(char *startDir, struct LocalTracksArray* localTracksArray) {

	DIR* dirPtr = opendir(startDir);
	if (dirPtr == NULL) {
		// TODO: Implement errno
		fprintf(stderr, "Error opening directory: %s.\n", startDir);
		exit(1);
	}
	
	struct dirent* dirEntry;
	// Check next entry in directory
	while ((dirEntry = readdir(dirPtr)) != NULL) {

		char* name = dirEntry->d_name;

		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
			continue;
		}

		char fullPath[MAXIMUM_PATH_LENGTH + 1];
		concatPath(fullPath, startDir, name, MAXIMUM_PATH_LENGTH);
		enum AnalyzedType anType = getFileTypeForPath(fullPath);

		// If found a directory
		if (anType == ANALYZED_TYPE_DIRECTORY) {
			recursiveTrackSearch(fullPath, localTracksArray);
			continue;
		}
		// If found a file
		if (anType == ANALYZED_TYPE_FILE) {

			char fileExtension[6];
			getFileExtension(fileExtension, name);

			// If supported file extension
			if ((strcmp(fileExtension, "mp3") == 0) || (strcmp(fileExtension, "flac") == 0)) {

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

			// Else: Print info for unsupported file types
			} else {
				// But skip Rekordbox's own sample files
				bool rekordboxSampleFound = 0;
				for (int i = 0; i < REKORDBOX_IGNORE_COUNT; i++) {
					if (strcmp(name, REKORDBOX_IGNORE[i]) == 0) {
						rekordboxSampleFound = 1;
						break;
					}
				}
				if (rekordboxSampleFound) {
					continue;
				}
				if (fileExtension[0] == '\0') {
					continue;
				}
				for (int i = 0; i < FILE_EXTENSIONS_FOR_WARNING_COUNT; i++) {
					if (strcmp(fileExtension, FILE_EXTENSIONS_FOR_WARNING[i]) == 0) {
						printf("Info: Ignoring file '%s' due to its file type.\n", fullPath);
						break;
					}
				}
			}
		}
	}
	closedir(dirPtr);
}