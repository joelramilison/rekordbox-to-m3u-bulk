#include "file_io_general.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
