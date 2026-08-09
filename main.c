#include "config.h"
#include "structs.h"
#include "parser.h"
#include "file_reader.h"

int main(void) {

	// Load in collection tracks
	struct CollectionTrack* collectionTracks;
	int collectionTracksCount = parseXml(collectionTracks);	

	struct LocalTracksArray localTracksArray = {0};
	localTracksArray.allocatedBytes = 1000 * sizeof(struct LocalTrack);
	localTracksArray.array = malloc(localTracksArray.allocatedBytes);
	recursiveTrackSearch((char* ) LOCAL_FILES_ROOT_DIRECTORY, &localTracksArray);
	return 0;
}
