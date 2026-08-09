#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include "config.h"
#include <stdbool.h>

struct CollectionTrack {
	
	char title[MAXIMUM_TITLE_LENGTH + 1];
	char artist[MAXIMUM_ARTIST_LENGTH + 1];
	char path[MAXIMUM_PATH_LENGTH + 1];
	char trackId[MAXIMUM_TRACK_ID_LENGTH + 1];
};

struct CollectionTracksArray {

	struct CollectionTrack* array;
	size_t tracksCount;
};

struct LocalTrack {

	char title[MAXIMUM_TITLE_LENGTH + 1];
	char artist[MAXIMUM_ARTIST_LENGTH + 1];
	char path[MAXIMUM_PATH_LENGTH + 1];
};

struct LocalTracksArray {

	struct LocalTrack* array;
	size_t tracksCount;
	size_t allocatedBytes;
};

// If isPlaylist is 1, can't use 'tracks'. If it's 0, can't use 'childrenNodes'.
struct PlaylistNode {
    char name[MAXIMUM_PLAYLIST_NODE_NAME_LENGTH + 1];
	char** trackIds;
    struct PlaylistNode* childrenNodes;
	bool isPlaylist;
	size_t count;
};

#endif