#include "matching.h"
#include "structs.h"
#include <string.h>
#include "config.h"
#include <stdio.h>

// returns count of found paths
int findPathsByTags(char*** dest, size_t* capacity, char* title, char* artist,
    struct LocalTracksArray* local) {
    int found = 0;
    for (int i = 0; i < local->tracksCount; i++) {
        if (strcmp(local->array[i].title, title) == 0 && strcmp(local->array[i].artist, artist) == 0) {
            found += 1;
            if ((size_t)found > *capacity) {
                size_t oldCapacity = *capacity;
                *capacity = *capacity * 2;
                *dest = realloc(*dest, *capacity * sizeof(char*));
                for (int j = oldCapacity; (size_t) j < *capacity; j++) {
                    printf("malloc for j = %d ...\n", j);
                    (*dest)[j] = malloc(MAXIMUM_PATH_LENGTH + 1);
                }
            }
            strlcpy((*dest)[found - 1], local->array[i].path, MAXIMUM_PATH_LENGTH + 1);
        }
    }
    return found;
}

// returns 1 if found exactly 1 path for every collection track
bool addPathsToCollection(struct LocalTracksArray* local, struct CollectionTracksArray* collection) {

    bool perfectSuccess = 1;
    size_t startCapacity = 50;
    char** dest = malloc(startCapacity * sizeof(char*));
    for (int i = 0; i < startCapacity; i++) {
        dest[i] = malloc(MAXIMUM_PATH_LENGTH + 1);
    }
    for (int i = 0; i < collection->tracksCount; i++) {
        int found = findPathsByTags(&dest, &startCapacity, collection->array[i].title,
            collection->array[i].artist, local);
        if (found == 0) {
            fprintf(stderr, "Matching error: Couldn't find paths for track {\nTitle: %s\nArtist: %s\n}\n",
                collection->array[i].title, collection->array[i].artist);
            perfectSuccess = 0;
            continue;
        }
        if (found == 1) {
            strlcpy(collection->array[i].path, dest[0], MAXIMUM_PATH_LENGTH + 1);
            continue;
        }
        fprintf(stderr, "Matching error: Found more than 1 path for track {\nTitle: %s\nArtist: %s\n}",
            collection->array[i].title, collection->array[i].artist);
        fprintf(stderr, "Found paths: {\n");
        for (int j = 0; j < found; j++) {
            fprintf(stderr, "%s\n", dest[j]);
        }
        fprintf(stderr, "\n");
    }
    

    // Optional freeing, but not needed since it's a short script:
    //for (int i = 0; i < capacity; i++) {
    //    free(dest[i]);
    //}
    //free(dest);
    return perfectSuccess;
}