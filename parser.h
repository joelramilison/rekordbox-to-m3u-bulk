#ifndef PARSER_H
#define PARSER_H

#include "structs.h"
#include <libxml/tree.h>

void findPlaylists(xmlNodePtr cur, struct PlaylistNode* plNodePtr);

void importPlaylist(xmlNodePtr cur, struct PlaylistNode* plNodePtr);

int parseXml(struct CollectionTrack** collectionTracks, struct PlaylistNode* plNodePtr);

int parseCollectionTracks(struct CollectionTrack** collectionTracksPtr, xmlNodePtr cur);

#endif