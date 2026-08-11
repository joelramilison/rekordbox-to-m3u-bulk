#include "playlist_writing.h"
#include "structs.h"
#include <stdio.h>
#include <string.h>

struct CollectionTrack* findTrackById(const char* trackId, const struct CollectionTracksArray* collectionTracks) {

    for (int i = 0; i < collectionTracks->tracksCount; i++) {

        if (strcmp(trackId, collectionTracks->array[i].trackId) == 0) {
            return &(collectionTracks->array[i]);
        }
    }
    return NULL;
}

void writePlaylist(const struct PlaylistNode* plNode, FILE* file, const struct CollectionTracksArray* collectionTracks) {

    for (int i = 0; i < plNode->count; i++) {
        
        char* trackId = plNode->trackIds[i];
        struct CollectionTrack* track = findTrackById(trackId, collectionTracks);
        if (track == NULL) {
            fprintf(stderr, "Playlist writing error: Couldn't find track for trackId %s.\n", trackId);
            exit(1);
        }
        
        fputs(track->path, file);
        if (i < plNode->count - 1) {
            fputs("\n", file);
        }

    }
}

void exportAllPlaylists(const struct PlaylistNode* plNode, const struct CollectionTracksArray* collectionTracks) {


}