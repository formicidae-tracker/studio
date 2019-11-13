#pragma once

#include <myrmidon/utils/FileSystem.hpp>
#include <memory>
#include <iostream>

#include <fort-hermes/FrameReadout.pb.h>

#include <google/protobuf/util/time_util.h>

#include "../Time.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class TrackingDataDirectory;

// RawFrame stores raw tracking data in a point in Time
//
//
// RawFrames stores all tracking data in a point in time. It has an
// associated <Time> storing both a Wall time given by the tracking
// computer when the frame was acquired, and a monotonic timestamp of
// the framegrabber.
//
class RawFrame {
public:
	// A const pointer to a RawFrame
	typedef std::shared_ptr<const RawFrame> ConstPtr;


	const fs::path & Basepath() const;

	// A Path uniquely defining the FramePointer
	// @return a fs::path uniquely identifying the Frame
	fs::path Path() const;

	// The FrameID
	inline uint64_t FrameID() const {
		return d_frame;
	}

	// The corresponding Time
	const fort::myrmidon::Time & Time() const;

	// Any error on the frame
	fort::hermes::FrameReadout_Error Error() const;

	// The width of the frame
	int32_t Width() const;

	// The height of the frame
	int32_t Height() const;

	const ::google::protobuf::RepeatedPtrField<::fort::hermes::Tag> & Tags() const;


	static RawFrame::ConstPtr Create(const fs::path & path,
	                                 fort::hermes::FrameReadout & pb,
	                                 Time::MonoclockID clockID);

private:

	RawFrame(const fs::path & path,
	         fort::hermes::FrameReadout & pb,
	         Time::MonoclockID clockID);

	fs::path              d_path;
	fort::myrmidon::Time  d_time;


	uint64_t                                              d_frame;
	fort::hermes::FrameReadout_Error                      d_error;
	int32_t                                               d_width,d_height;
	google::protobuf::RepeatedPtrField<fort::hermes::Tag> d_tags;
};

}

}

}

// Formats a FramePointer
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::FramePointer> to format
// @return a reference to <out>
std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::RawFrame & p);
