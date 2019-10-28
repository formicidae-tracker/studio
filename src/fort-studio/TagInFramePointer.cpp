#include "TagInFramePointer.hpp"

#include <sstream>


TagInFramePointer::TagInFramePointer(const fort::myrmidon::priv::FramePointer::Ptr& frame,
                                     uint32_t tagValue)
	: d_frame(frame)
	, d_tagValue(tagValue) {

}

TagInFramePointer::~TagInFramePointer() {}

const fort::myrmidon::priv::FramePointer::Ptr& TagInFramePointer::Frame() const {
	return d_frame;
}

uint32_t TagInFramePointer::TagValue() const {
	return d_tagValue;
}

fs::path TagInFramePointer::Path() const {
	std::ostringstream os;
	os << d_tagValue;
	return d_frame->FullPath() / os.str();
}

fs::path TagInFramePointer::Base() const {
	return d_frame->Path;
}
