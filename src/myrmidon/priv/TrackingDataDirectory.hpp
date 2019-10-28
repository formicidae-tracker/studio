#pragma once

#include "ForwardDeclaration.hpp"

#include <myrmidon/utils/FileSystem.hpp>

#include <google/protobuf/util/time_util.h>

namespace fort {

namespace myrmidon {

namespace pb {
class TrackingDataDirectory;
}

namespace priv {

class TrackingDataDirectory {
public:
	TrackingDataDirectory();
	TrackingDataDirectory(const fs::path & path,
	                      uint64_t startFrame,
	                      uint64_t endFrame,
	                      const google::protobuf::Timestamp & start,
	                      const google::protobuf::Timestamp & end);


	const fs::path &  Path() const;

	uint64_t StartFrame() const;
	uint64_t EndFrame() const;

	const google::protobuf::Timestamp & StartDate() const;
	const google::protobuf::Timestamp & EndDate() const;

	FramePointerPtr FramePointer(uint64_t frame) const;
	FramePointerPtr FramePointer(const fs::path & path) const;


	static TrackingDataDirectory Open(const fs::path & path, const fs::path & base);




private:
	fs::path                    d_path;
	uint64_t                    d_startFrame,d_endFrame;
	google::protobuf::Timestamp d_startDate,d_endDate;


};

} //namespace priv

} //namespace myrmidon

} //namespace fort


inline bool operator<(const fort::myrmidon::priv::TrackingDataDirectory & a,
                      const fort::myrmidon::priv::TrackingDataDirectory & b) {
	return a.StartDate() < b.StartDate();
}
