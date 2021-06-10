#include "TrackingSolver.hpp"

#include <fort/myrmidon/priv/TrackingSolver.hpp>


namespace fort {
namespace myrmidon {


TrackingSolver::TrackingSolver(const PPtr & pTracker)
	: d_p(pTracker) {
}

IdentifiedFrame::Ptr TrackingSolver::IdentifyFrame(const fort::hermes::FrameReadout & frame,
                                                   SpaceID spaceID) const {
	return d_p->IdentifyFrame(frame,spaceID);
}

CollisionFrame::Ptr TrackingSolver::CollideFrame(const IdentifiedFrame::Ptr & identified) const {
	return d_p->CollideFrame(identified);
}

AntID TrackingSolver::IdentifyTag(TagID tagID, const Time & time) {
	return d_p->IdentifyTag(tagID,time);
}


} // namespace myrmidon
} // namespace fort
