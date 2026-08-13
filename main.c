#include "config.h"
#include "structs.h"
#include "parser.h"
#include "file_reader.h"
#include "matching.h"
#include "file_io_general.h"
#include "playlist_writing.h"

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
	// Only start writing the .m3u playlists if found exactly 1 filepath for every collection track
	if (!perfectSuccess) {
		return 0;
	}

	exportAllPlaylists(GENERATE_PLAYLISTS_DIRECTORY, &plNode, &collectionTracks);

	printf("Successfully exported all playlists.\n");

	return 0;
}
