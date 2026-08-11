#include "file_reader.h"
#include <stdio.h>
#include "structs.h"
#include "config.h"
#include <dirent.h>
#include <taglib/tag_c.h>
#include <string.h>
#include <errno.h>
#include "file_io_general.h"

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