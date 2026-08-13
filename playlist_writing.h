#ifndef PLAYLIST_WRITING_H
#define PLAYLIST_WRITING_H

#include "structs.h"

void exportAllPlaylists(const char* directory, struct PlaylistNode* rootNode, const struct CollectionTracksArray* collectionTracks);

#endif