#pragma once

#include <memory>
#include <iostream>

#include <fort/hermes/FrameReadout.pb.h>

#include <google/protobuf/util/time_util.h>

#include <fort/myrmidon/Time.hpp>
#include "Types.hpp"
#include "LocatableTypes.hpp"
#include "FrameReference.hpp"




namespace fort {
namespace myrmidon {
namespace priv {

class IdentifierIF;
class TrackingDataDirectory;

// RawFrame stores raw tracking data in a point in Time
//
//
// RawFrames stores all tracking data in a point in time. It has an
// associated <Time> storing both a Wall time given by the tracking
// computer when the frame was acquired, and a monotonic timestamp of
// the framegrabber.
//
class RawFrame : public Identifiable {
public:
	virtual ~RawFrame();
	// A const pointer to a RawFrame
	typedef std::shared_ptr<const RawFrame> ConstPtr;

	// Any error on the frame
	fort::hermes::FrameReadout_Error Error() const;

	const std::string & URI() const override;

	const FrameReference & Frame() const;

	// The width of the frame
	int32_t Width() const;

	// The height of the frame
	int32_t Height() const;

	const ::google::protobuf::RepeatedPtrField<::fort::hermes::Tag> & Tags() const;

	IdentifiedFrame::Ptr IdentifyFrom(const IdentifierIF & identifier,SpaceID spaceID) const;

	static RawFrame::ConstPtr Create(const std::string & parentURI,
	                                 fort::hermes::FrameReadout & pb,
	                                 Time::MonoclockID clockID);

private:

	RawFrame(const std::string & parentURI,
	         fort::hermes::FrameReadout & pb,
	         Time::MonoclockID clockID);


	fort::hermes::FrameReadout_Error                      d_error;
	int32_t                                               d_width,d_height;
	google::protobuf::RepeatedPtrField<fort::hermes::Tag> d_tags;
	FrameReference                                        d_frame;
	std::string                                           d_URI;
};

}
}
}
