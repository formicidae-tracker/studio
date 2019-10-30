#pragma once

#include <myrmidon/utils/FileSystem.hpp>
#include <memory>
#include <iostream>

#include <google/protobuf/util/time_util.h>


namespace fort {

namespace myrmidon {

namespace priv {

// Identifies an Experimental Point in Time
//
// Identifies an Experimental Point in Time. This class should either
// be renamed, deleted or moved to the <mymidon> namespace.
class FramePointer {
public:

	// A pointer to FramePointer (Ugly)
	typedef std::shared_ptr<const FramePointer> Ptr;
	// A const pointer to FramePointer (Ugly)
	typedef std::shared_ptr<const FramePointer> ConstPtr;

	// The path to the parent TrackingDataDirectory
	fs::path                    Path;

	// The StartingDate of the parent TrackingDataDirectory
	google::protobuf::Timestamp PathStartDate;

	// The frame number
	uint64_t                    Frame;

	// A Path uniquely defining the FramePointer
	// @return a fs::path uniquely identifying the Frame
	fs::path                    FullPath() const;
};

}

}

}

// Allows FramePointer to be strictly ordered
// @a the first <fort::myrmidon::priv::FramePointer>
// @b the first <fort::myrmidon::priv::FramePointer>
// @return true if a happens strictly before b
inline bool operator<(const fort::myrmidon::priv::FramePointer & a,
                      const fort::myrmidon::priv::FramePointer & b) {
	if (a.Path == b.Path) {
		return a.Frame < b.Frame;
	}
	return a.PathStartDate < b.PathStartDate;
}

// Allows FramePointer to be weakly ordered
// @a the first <fort::myrmidon::priv::FramePointer>
// @b the first <fort::myrmidon::priv::FramePointer>
// @return true if a happens before or at the same time tham b
inline bool operator<=(const fort::myrmidon::priv::FramePointer & a,
                      const fort::myrmidon::priv::FramePointer & b) {
	if (a.Path == b.Path) {
		return a.Frame <= b.Frame;
	}
	return a.PathStartDate <= b.PathStartDate;
}

// Allows FramePointer to be differenciated
// @a the first <fort::myrmidon::priv::FramePointer>
// @b the first <fort::myrmidon::priv::FramePointer>
// @return true if a happens before or at the same time tham b

inline bool operator!=(const fort::myrmidon::priv::FramePointer & a,
                       const fort::myrmidon::priv::FramePointer & b) {
	return a.Path != b.Path || a.Frame != b.Frame;
}


// Formats a FramePointer
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::FramePointer> to format
// @return a reference to <out>
inline std::ostream& operator<<(std::ostream & out,
                                const fort::myrmidon::priv::FramePointer & p) {
	return out << p.Path.generic_string() << "/" << p.Frame;
}
