#ifndef PARSER_H
#define PARSER_H

#include "structs.h"
#include <libxml/tree.h>

void findPlaylists(xmlNodePtr cur, struct PlaylistNode* plNodePtr);

void importPlaylist(xmlNodePtr cur, struct PlaylistNode* plNodePtr);

void parseXml(struct CollectionTracksArray* collectionTracksArrayPtr, struct PlaylistNode* plNodePtr);

void parseCollectionTracks(struct CollectionTracksArray* collectionTracksArrayPtr, xmlNodePtr cur);

#endif