#include "TagCloseUp.hpp"

#include <regex>

#include <myrmidon/utils/Checker.hpp>
#include <myrmidon/priv/proto/FileReadWriter.hpp>
#include <myrmidon/priv/proto/IOUtils.hpp>
#include <myrmidon/TagCloseUpCache.pb.h>


namespace fort {
namespace myrmidon {
namespace priv {


TagCloseUp::TagCloseUp(const fs::path & absoluteFilePath,
                       const FrameReference & reference,
                       TagID tid,
                       const Eigen::Vector2d & position,
                       double angle,
                       const Vector2dList & corners)
	: d_reference(reference)
	, d_URI(d_reference.URI() / "closeups" / std::to_string(tid))
	, d_absoluteFilePath(absoluteFilePath)
	, d_tagID(tid)
	, d_tagPosition(position)
	, d_tagAngle(angle)
	, d_corners(corners) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);
}

TagCloseUp::~TagCloseUp() {}

const FrameReference & TagCloseUp::Frame() const {
	return d_reference;
}

const fs::path & TagCloseUp::URI() const {
	return d_URI;
}

const fs::path & TagCloseUp::AbsoluteFilePath() const  {
	return d_absoluteFilePath;
}

TagID TagCloseUp::TagValue() const {
	return d_tagID;
}

const Eigen::Vector2d & TagCloseUp::TagPosition() const {
	return d_tagPosition;
}

double TagCloseUp::TagAngle() const {
	return d_tagAngle;
}

const TagCloseUp::Vector2dList & TagCloseUp::Corners() const {
	return d_corners;
}


std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>>
TagCloseUp::ListFiles(const fs::path & path) {
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

std::vector<TagCloseUp::Loader> TagCloseUp::PrepareLoading(const fs::path & absoluteFilePath,
                                                           fort::tags::Family f,
                                                           uint8_t threshold) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);

	auto cache = std::make_shared<TagCloseUpByURI>();
	typedef  proto::FileReadWriter<pb::TagCloseUpCacheHeader,pb::TagCloseUp> RW;

	try {
		RW::Read(absoluteFilePath / "tag-close-up.cache",
		         [=](const pb::TagCloseUpCacheHeader & h) {
			         if ( h.threshold() ) {

			         }
		         }


	} catch ( const std::exeception & e) {

	}



}


} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagCloseUp & p) {
	return out << p.Frame() << "/closeups/" << p.TagValue();
}
