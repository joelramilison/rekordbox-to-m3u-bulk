#include "playlist_writing.h"
#include "structs.h"
#include <stdio.h>
#include <string.h>
#include "file_io_general.h"
#include <sys/stat.h>

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

void exportRecursion(const char* directory, struct PlaylistNode* node, const struct CollectionTracksArray* collectionTracks) {

    if (node->isPlaylist) {

        char playlistPath[MAXIMUM_PATH_LENGTH + 1];
        
        // Replace forbidden characters in filename
        for (char* p = node->name; *p != '\0'; p++) {
            if (*p == '/') {
                *p = '_';
            }
        }
        
        concatPath(playlistPath, directory, node->name, MAXIMUM_PATH_LENGTH + 1);
        strlcat(playlistPath, ".m3u", MAXIMUM_PATH_LENGTH + 1);
        FILE* file = fopen(playlistPath, "w");
        if (file == NULL) {
            fprintf(stderr, "File I/O Error: Couldn't create file '%s'.\n", playlistPath);
            exit(1);
        }
        writePlaylist(node, file, collectionTracks);
        fclose(file);

    // If Playlist folder: Create directory and traverse each child node
    } else {

        char playlistFolderPath[MAXIMUM_PATH_LENGTH + 1];
        concatPath(playlistFolderPath, directory, node->name, MAXIMUM_PATH_LENGTH + 1);

        if (mkdir(playlistFolderPath, 0755) != 0) {
            fprintf(stderr, "File I/O Error: Couldn't create directory '%s'.\n", playlistFolderPath);
            exit(1);
        }

        for (int i = 0; i < node->count; i++) {
            exportRecursion(playlistFolderPath, &(node->childrenNodes[i]), collectionTracks);
        }
    }
}

void exportAllPlaylists(const char* directory, struct PlaylistNode* rootNode, const struct CollectionTracksArray* collectionTracks) {

    struct stat st;
    // Create root folder if doesn't exist. Otherwise clear it.
    if (stat(directory, &st) == -1) {
         if (mkdir(directory, 0755) != 0) {
            fprintf(stderr, "File I/O Error: Couldn't create root directory '%s'.\n", directory);
            exit(1);
        }
    } else {
        clearDirRecursively(directory);
    }

    for (int i = 0; i < rootNode->count; i++) {
        exportRecursion(directory, rootNode->childrenNodes + i, collectionTracks);
    }
}