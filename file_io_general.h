#ifndef FILE_IO_GENERAL_H
#define FILE_IO_GENERAL_H

enum AnalyzedType {
    ANALYZED_TYPE_FILE,
    ANALYZED_TYPE_DIRECTORY,
    ANALYZED_TYPE_OTHER
};

enum AnalyzedType getFileTypeForPath(char* path);

#endif