#include "RawFrame.hpp"

#include <sstream>

#include "../utils/NotYetImplemented.hpp"

#include "TimeUtils.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

RawFrame::~RawFrame() {}

fort::hermes::FrameReadout_Error RawFrame::Error() const {
	return d_error;
}

int32_t RawFrame::Width() const {
	return d_width;
}

int32_t RawFrame::Height() const {
	return d_height;
}

const ::google::protobuf::RepeatedPtrField<::fort::hermes::Tag> & RawFrame::Tags() const {
	return d_tags;
}


RawFrame::ConstPtr RawFrame::Create(const std::string & URI,
                                    fort::hermes::FrameReadout & pb,
                                    Time::MonoclockID clockID) {
	return std::shared_ptr<const RawFrame>(new RawFrame(URI,pb,clockID));
}


RawFrame::RawFrame(const std::string & URI,
                   fort::hermes::FrameReadout & pb,
                   Time::MonoclockID clockID)
	: d_error(pb.error())
	, d_width(pb.width())
	, d_height(pb.height())
	, d_frame(URI,pb.frameid(),TimeFromFrameReadout(pb, clockID))
	, d_URI( (fs::path(d_frame.URI()) / "rawdata").generic_string() ) {
	d_tags.Swap(pb.mutable_tags());
}

const std::string & RawFrame::URI() const {
	return d_URI;
}

const FrameReference & RawFrame::Frame() const {
	return d_frame;
}

} //namespace priv
} //namespace myrmidon
} //namespace fort
