#include "config.h"
#include "structs.h"
#include "parser.h"
#include "file_reader.h"

int main(void) {

	// Load in collection tracks from Rekordbox Collection XML
	struct CollectionTracksArray collectionTracks = {0};
	struct PlaylistNode plNode = {0};
	parseXml(&collectionTracks, &plNode);

	// Scan MP3 tags from filesystem by recursively searching a directory
	struct LocalTracksArray localTracksArray = {0};
	recursiveTrackSearch((char* ) LOCAL_FILES_ROOT_DIRECTORY, &localTracksArray);
	
	return 0;
}
