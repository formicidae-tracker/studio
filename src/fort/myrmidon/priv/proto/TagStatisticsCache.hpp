#pragma once

#include "FileReadWriter.hpp"

#include <fort/myrmidon/priv/TagStatistics.hpp>
#include <fort/myrmidon/TagStatisticsCache.pb.h>

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

class TagStatisticsCache {
public:
	typedef FileReadWriter<pb::TagStatisticsCacheHeader,pb::TagStatistics> ReadWriter;
	static TagStatisticsHelper::Timed Load(const fs::path & tddAbsolutePath);

	static void Save(const fs::path & tddAbsolutePath,const TagStatisticsHelper::Timed & stats);

	const static std::string CACHE_PATH;

	const static uint32_t CACHE_VERSION;
};


} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
