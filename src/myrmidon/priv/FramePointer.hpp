#pragma once

#include <filesystem>
#include <memory>
#include <iostream>


#include "Experiment.pb.h"
#include <google/protobuf/util/time_util.h>


namespace fort {

namespace myrmidon {

namespace priv {

class FramePointer {
public:
	typedef std::shared_ptr<FramePointer> Ptr;
	std::filesystem::path       Path;
	google::protobuf::Timestamp PathStartDate;
	uint64_t                    Frame;

	void Encode(fort::myrmidon::pb::FramePointer & fp) const;
	static FramePointer::Ptr FromSaved(const fort::myrmidon::pb::FramePointer & fp);
};

}

}

}

inline bool operator<(const fort::myrmidon::priv::FramePointer & a,
                      const fort::myrmidon::priv::FramePointer & b) {
	if (a.Path == b.Path) {
		return a.Frame < b.Frame;
	}
	return a.PathStartDate < b.PathStartDate;
}
