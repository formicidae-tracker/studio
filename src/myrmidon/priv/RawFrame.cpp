#include "RawFrame.hpp"

#include <sstream>

#include "../utils/NotYetImplemented.hpp"

#include "TimeUtils.hpp"

#include "Identifier.hpp"
#include "Isometry2D.hpp"
#include "Identification.hpp"
#include "Ant.hpp"


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

IdentifiedFrame::ConstPtr RawFrame::IdentifyFrom(const IdentifierIF & identifier) const {
	auto res = std::make_shared<IdentifiedFrame>();
	res->FrameTime = Frame().Time();
	res->Width = d_width;
	res->Height = d_height;
	res->Positions.reserve(d_tags.size());
	for ( const auto & t : d_tags ) {
		auto identification = identifier.Identify(t.id(),res->FrameTime);
		if ( !identification ) {
			continue;
		}
		Isometry2Dd tagToOrig(t.theta(),Eigen::Vector2d(t.x(),t.y()));
		auto antToOrig = identification->AntToTagTransform() * tagToOrig;
		res->Positions.push_back({antToOrig.translation(),antToOrig.angle(),identification->Target()->ID()});
	}
	return res;
}


} //namespace priv
} //namespace myrmidon
} //namespace fort
