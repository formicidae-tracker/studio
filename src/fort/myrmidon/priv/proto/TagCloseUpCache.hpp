#pragma once

#include "FileReadWriter.hpp"

#include <fort/myrmidon/priv/TagCloseUp.hpp>
#include <fort/myrmidon/TagCloseUpCache.pb.h>

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

class TagCloseUpCache {
public:
	typedef FileReadWriter<pb::TagCloseUpCacheHeader,pb::TagCloseUp> ReadWriter;
	static std::vector<TagCloseUp::ConstPtr> Load(const fs::path & tddAbsoluteFilePath,
	                                              std::function<FrameReference (FrameID)> resolver);

	static void Save(const fs::path & tddAbsoluteFilePath,const std::vector<TagCloseUp::ConstPtr> & tagCloseUps);

	const static std::string CACHE_PATH;

	const static uint32_t CACHE_VERSION;
};


} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
