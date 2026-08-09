#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdlib.h>
#include "structs.h"

char* concatPath(char* fullPath, const char* first, const char* second, size_t maxLen);

void recursiveTrackSearch(char *startDir, struct LocalTracksArray* localTracksArray);

#endif