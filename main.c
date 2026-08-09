#include "config.h"
#include "structs.h"
#include "parser.h"
#include "file_reader.h"
#include "matching.h"

int main(void) {

	// Load in collection tracks from Rekordbox Collection XML
	struct CollectionTracksArray collectionTracks = {0};
	struct PlaylistNode plNode = {0};
	parseXml(&collectionTracks, &plNode);

	// Scan MP3 tags from filesystem by recursively searching a directory
	struct LocalTracksArray localTracks = {0};
	localTracks.allocatedBytes = 1000 * sizeof(struct LocalTrack);
	localTracks.array = malloc(localTracks.allocatedBytes);
	recursiveTrackSearch((char* ) LOCAL_FILES_ROOT_DIRECTORY, &localTracks);
	
	
	bool perfectSuccess = addPathsToCollection(&localTracks, &collectionTracks);

	return 0;
}
