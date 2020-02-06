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


RawFrame::ConstPtr RawFrame::Create(const fs::path & path,
                                    fort::hermes::FrameReadout & pb,
                                    Time::MonoclockID clockID) {
	return std::shared_ptr<const RawFrame>(new RawFrame(path,pb,clockID));
}


RawFrame::RawFrame(const fs::path & path,
                   fort::hermes::FrameReadout & pb,
                   Time::MonoclockID clockID)
	: FrameReference(path,pb.frameid(),TimeFromFrameReadout(pb, clockID))
	, d_error(pb.error())
	, d_width(pb.width())
	, d_height(pb.height()) {
	d_tags.Swap(pb.mutable_tags());
}


} //namespace priv
} //namespace myrmidon
} //namespace fort
