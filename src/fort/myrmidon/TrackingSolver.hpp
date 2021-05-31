#pragma once

#include <fort/hermes/FrameReadout.pb.h>

#include "Types.hpp"
#include "Ant.hpp"
#include "Space.hpp"



namespace fort {
namespace myrmidon {


namespace priv {
// private <fort::myrmidon::priv> Implementation
class TrackingSolver;
} // namespace priv


// Identifies and Collides Ant from raw tracking data
//
// This class lets the user to manually identify and track ants from
// raw tracking data, as for example, obtained from a network stream
// with `fort::hermes`

class TrackingSolver {
public :

	// Identifies a single tag
	// @tagID the TagID to identify
	// @time the time to consider to identify the tag
	//
	// @return 0 if the tag is not idnetified, or the corresponding ID
	AntID IdentifyTag(TagID tagID, const Time & time);

	// Identifies Ants from a `fort::hermes::FrameReadout`
	// @frame the `fort::hermes::FrameReadout` to identify
	// @spaceID the spaceID the frame correspond to
	//
	// @return an <IdentifiedFrame> with all identified ant (without zone)
	IdentifiedFrame::Ptr IdentifyFrame(const fort::hermes::FrameReadout & frame,
	                                   Space::ID spaceID) const;

	// Collides Ants from an IdentifiedFrame
	// @identified the <IdentifiedFrame> with the ant position data.
	//
	// Collides Ants from an <IdentifiedFrame>. <identified> will be
	// modified to contains for each Ant its current zone.
	//
	// @return a <CollisionFrame> with all current Ant collisions.
	CollisionFrame::ConstPtr CollideFrame(const IdentifiedFrame::Ptr & identified) const;

	// Opaque pointer to implementation
	typedef const std::shared_ptr<priv::TrackingSolver> PPtr;

	// Private implementation constructor
	// @pTracker opaque pointer to implementation
	//
	// User cannot create a TrackingSolver directly. They must use
	// <Experiment::CompileTrackingSolver>.
	TrackingSolver(const PPtr & pTracker);
private:

	PPtr d_p;
};



} //namespace myrmidon
} //namespace fort
