#include "SnapshotCache.hpp"

#include <algorithm>
#include <cctype>
#include <regex>

namespace fort {
namespace myrmidon {
namespace priv {

std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>>
SnapshotIndex::ListSnapshotFiles(const fs::path & path) {
	std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>> res;

	static std::regex singleRx("ant_([0-9]+)_frame_([0-9]+).png");
	static std::regex multiRx("frame_([0-9]+).png");

	for ( const auto & de : fs::directory_iterator(path) ) {
		auto ext = de.path().extension().string();
		std::transform(ext.begin(),ext.end(),ext.begin(),
		               [](unsigned char c) {
			               return std::tolower(c);
		               });
		if ( ext != ".png" ) {
			continue;
		}

		std::smatch ID;
		std::string filename = de.path().filename().string();
		FrameID FID;
		if(std::regex_search(filename,ID,singleRx) && ID.size() > 2) {
			std::istringstream IDS(ID.str(1));
			std::istringstream FrameS(ID.str(2));
			auto TID = std::make_shared<TagID>(0);

			IDS >> *(TID);
			FrameS >> FID;
			res.insert(std::make_pair(FID,std::make_pair(de.path(),TID)));
			continue;
		}
		if(std::regex_search(filename,ID,multiRx) && ID.size() > 1) {
			std::istringstream FrameS(ID.str(1));
			FrameS >> FID;
			res.insert(std::make_pair(FID,std::make_pair(de.path(),std::shared_ptr<TagID>())));
			continue;
		}

	}

	return res;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
