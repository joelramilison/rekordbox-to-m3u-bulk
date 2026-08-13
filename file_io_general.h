#ifndef FILE_IO_GENERAL_H
#define FILE_IO_GENERAL_H

#include <stdlib.h>

enum AnalyzedType {
    ANALYZED_TYPE_FILE,
    ANALYZED_TYPE_DIRECTORY,
    ANALYZED_TYPE_OTHER
};

enum AnalyzedType getFileTypeForPath(char* path);

// Create full path out of first and second, adding or removing '/' in between them as needed.
// Pass a string pointer to pospulate.
// size is the buffer size of fullPath.
void concatPath(char* fullPath, const char* first, const char* second, size_t size);

void getFileExtension(char* dest, char* fileName);

// Recursively removes all the directories and files inside the specified directory.
void clearDirRecursively(const char* path);

#endif