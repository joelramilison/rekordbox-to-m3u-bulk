#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include "config.h"

struct CollectionTrack {
	
	char title[MAXIMUM_TITLE_LENGTH + 1];
	char artist[MAXIMUM_ARTIST_LENGTH + 1];
	char path[MAXIMUM_PATH_LENGTH + 1];
	char trackId[MAXIMUM_TRACK_ID_LENGTH + 1];
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

#endif