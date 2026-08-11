#ifndef FILE_IO_GENERAL_H
#define FILE_IO_GENERAL_H

#include <stdlib.h>

enum AnalyzedType {
    ANALYZED_TYPE_FILE,
    ANALYZED_TYPE_DIRECTORY,
    ANALYZED_TYPE_OTHER
};

enum AnalyzedType getFileTypeForPath(char* path);

char* concatPath(char* fullPath, const char* first, const char* second, size_t maxLen);

void getFileExtension(char* dest, char* fileName);

#endif