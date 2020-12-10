#pragma once

#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/TrackingDataDirectory.pb.h>

#include "FileReadWriter.hpp"

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

class TDDCache {
public:
	typedef FileReadWriter<pb::TrackingDataDirectory,pb::TrackingDataDirectoryFileLine> ReadWriter;
	static TrackingDataDirectory::Ptr Load(const fs::path & absoluteFilePath ,
	                                       const std::string & URI);

	static void Save(const TrackingDataDirectory::Ptr & tdd);

	const static std::string CACHE_FILENAME;
	const static uint32_t CACHE_VERSION;
};


} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
