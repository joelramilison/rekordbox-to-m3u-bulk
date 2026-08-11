#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

#define REKORDBOX_COLLECTION_XML_PATH "/Users/joelramilison/Documents/Rekordbox_Collection.xml"
// Directory to recursively look for music files
#define LOCAL_FILES_ROOT_DIRECTORY "/Users/joelramilison/Cloud Option DJ team Dropbox/Joel Ramilison/rekordbox/contents_1076899183"
#define GENERATE_PLAYLISTS_DIRECTORY "/Users/joelramilison/Music/Generated Playlists"
#define MAXIMUM_TITLE_LENGTH 150
#define MAXIMUM_ARTIST_LENGTH 150
#define MAXIMUM_TRACK_ID_LENGTH 20
#define MAXIMUM_PATH_LENGTH 1024
#define MAXIMUM_PLAYLIST_NODE_NAME_LENGTH 200
extern const char* const REKORDBOX_IGNORE[8];
#define REKORDBOX_IGNORE_COUNT 8
extern const char* const FILE_EXTENSIONS_FOR_WARNING[13];
#define FILE_EXTENSIONS_FOR_WARNING_COUNT 13


#endif