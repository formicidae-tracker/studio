#pragma once

#include <filesystem>

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
	TrackingDataDirectory(const std::filesystem::path & path,
	                      uint64_t startFrame,
	                      uint64_t endFrame,
	                      const google::protobuf::Timestamp & start,
	                      const google::protobuf::Timestamp & end);


	const std::filesystem::path &  Path() const;

	uint64_t StartFrame() const;
	uint64_t EndFrame() const;

	const google::protobuf::Timestamp & StartDate() const;
	const google::protobuf::Timestamp & EndDate() const;

	static TrackingDataDirectory Open(const std::filesystem::path & path, const std::filesystem::path & base);


private:
	std::filesystem::path       d_path;
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
