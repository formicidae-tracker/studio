#include "RawFrame.hpp"

#include <sstream>

#include "../utils/NotYetImplemented.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

const std::string & RawFrame::Basepath() const {
	return d_path;
}

fs::path RawFrame::Path() const {
	return fs::path(d_path) / std::to_string(d_frame);
}


const Time & RawFrame::Time() const {
	return d_time;
}

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


RawFrame::ConstPtr RawFrame::Create(const std::string & path,
                                    fort::hermes::FrameReadout & pb,
                                    Time::MonoclockID clockID) {
	return std::shared_ptr<const RawFrame>(new RawFrame(path,pb,clockID));
}


RawFrame::RawFrame(const std::string & path,
                   fort::hermes::FrameReadout & pb,
                   Time::MonoclockID clockID)
	: d_path(path)
	, d_time(Time::FromTimestampAndMonotonic(pb.time(), pb.timestamp() * 1000, clockID))
	, d_frame(pb.frameid())
	, d_error(pb.error())
	, d_width(pb.width())
	, d_height(pb.height()) {
	d_tags.Swap(pb.mutable_tags());
}


} //namespace priv
} //namespace myrmidon
} //namespace fort
std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::RawFrame & p) {
	return out << p.Basepath() << "/" << p.FrameID();
}
