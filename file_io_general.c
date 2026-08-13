#include "file_io_general.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "config.h"

enum AnalyzedType getFileTypeForPath(char* path) {
    
    struct stat st;
    if (stat(path, &st) != 0) {
        fprintf(stderr, "File error: Couldn't get information about the type of file/directory at path %s\n",
            path);
        exit(1);
    }

    if (S_ISDIR(st.st_mode)) {
        return ANALYZED_TYPE_DIRECTORY;
    }
    if (S_ISREG(st.st_mode)) {
        return ANALYZED_TYPE_FILE;
    }
    return ANALYZED_TYPE_OTHER;
}

void concatPath(char* fullPath, const char* first, const char* second, size_t size) {

	if(*first != '/') {
		fprintf(stderr, "String 1 has to begin with '/' to get a full path.");
		exit(1);
	}

	int counter = 0;
	while (*first) {
		fullPath[counter] = *first;
		first += 1;
		counter += 1;
		if (counter >= size) {
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
		if (counter >= size) {
			fprintf(stderr, "Strings too long for the maximum path length.");
			exit(1);
		}
	}
	fullPath[counter] = '\0';
	return;
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

void clearDirRecursively(const char* path) {

    DIR* dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "FILE I/O Error: Couldn't open directory at path %s\n", path);
        exit(1);
    }

    struct dirent* dirEntry;
    while ((dirEntry = readdir(dir)) != NULL) {

        if ((strcmp(dirEntry->d_name, ".") == 0) || (strcmp(dirEntry->d_name, "..") == 0)) {
            continue;
        }

        char fullPath[MAXIMUM_PATH_LENGTH + 1];
        concatPath(fullPath, path, dirEntry->d_name, MAXIMUM_PATH_LENGTH);
        enum AnalyzedType anType = getFileTypeForPath(fullPath);

        if (anType == ANALYZED_TYPE_FILE) {
            if (remove(fullPath) != 0) {
                fprintf(stderr, "File I/O error: Couldn't remove file at path %s\n", fullPath);
                exit(1);
            }
            continue;
        }
        if (anType == ANALYZED_TYPE_DIRECTORY) {
            clearDirRecursively(fullPath);
            remove(fullPath);
            continue;
        }
    }
    closedir(dir);
}