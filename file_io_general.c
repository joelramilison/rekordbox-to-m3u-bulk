#include "file_io_general.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

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