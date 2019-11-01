#pragma once

#include <myrmidon/utils/FileSystem.hpp>
#include <memory>
#include <iostream>

#include <google/protobuf/util/time_util.h>

#include "../Time.hpp"

class FramePointerUTest_CanBeFormatted_Test;

namespace fort {

namespace myrmidon {

namespace priv {

class TrackingDataDirectory;

// Identifies an Experimental Point in Time
//
// Identifies an Experimental Point in Time. This class should either
// be renamed, deleted or moved to the <mymidon> namespace.
class FramePointer {
public:

	// A const pointer to FramePointer (Ugly)
	typedef std::shared_ptr<const FramePointer> ConstPtr;

	uint64_t Frame() const;

	const fs::path & Basepath() const;


	// A Path uniquely defining the FramePointer
	// @return a fs::path uniquely identifying the Frame
	fs::path FullPath() const;

	// The corresponding Time
	const fort::myrmidon::Time & Time() const;

	static FramePointer::ConstPtr Create(const fs::path & path, uint64_t frame, const fort::myrmidon::Time & t);

private:


	FramePointer(const fs::path & path, uint64_t frame, const fort::myrmidon::Time & t);

	fs::path              d_path;

	uint64_t              d_frame;

	fort::myrmidon::Time  d_time;
};

}

}

}

// Formats a FramePointer
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::FramePointer> to format
// @return a reference to <out>
std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::FramePointer & p);
