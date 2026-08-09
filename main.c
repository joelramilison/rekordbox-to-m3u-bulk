#include "config.h"
#include "structs.h"
#include "parser.h"
#include "file_reader.h"

int main(void) {

	// Load in collection tracks
	struct CollectionTrack* collectionTracks;
	struct PlaylistNode plNode = {0};
	int collectionTracksCount = parseXml(&collectionTracks, &plNode);	

	struct LocalTracksArray localTracksArray = {0};
	localTracksArray.allocatedBytes = 1000 * sizeof(struct LocalTrack);
	localTracksArray.array = malloc(localTracksArray.allocatedBytes);
	recursiveTrackSearch((char* ) LOCAL_FILES_ROOT_DIRECTORY, &localTracksArray);
	return 0;
}
