#pragma once

#include <myrmidon/priv/TrackingDataDirectory.hpp>
#include <myrmidon/TrackingDataDirectory.pb.h>

#include "FileReadWriter.hpp"

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

class TDDCache {
public:
	typedef FileReadWriter<pb::TrackingDataDirectory,pb::TrackingDataDirectoryFileLine> ReadWriter;
	static TrackingDataDirectory::ConstPtr Load(const fs::path & absoluteFilePath ,
	                                            const std::string & URI);

	static void Save(const TrackingDataDirectory::ConstPtr & tdd);

	const static std::string CACHE_FILENAME;
};


} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
