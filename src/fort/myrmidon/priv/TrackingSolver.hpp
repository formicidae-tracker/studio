#pragma once

#include <fort/hermes/FrameReadout.pb.h>
#include <fort/myrmidon/Types.hpp>

#include "CollisionSolver.hpp"
#include "Identifier.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class TrackingSolver {
public :
	typedef std::shared_ptr<const TrackingSolver> ConstPtr;

	TrackingSolver(const std::shared_ptr<const Identifier> & identifier,
	               const CollisionSolver::ConstPtr & solver);

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
	                                   SpaceID spaceID) const;

	// Collides Ants from an IdentifiedFrame
	// @identified the <IdentifiedFrame> with the ant position data.
	//
	// Collides Ants from an <IdentifiedFrame>. <identified> will be
	// modified to contains for each Ant its current zone.
	//
	// @return a <CollisionFrame> with all current Ant collisions.
	CollisionFrame::ConstPtr CollideFrame(const IdentifiedFrame::Ptr & identified) const;
private :
	std::shared_ptr<const Identifier> d_rawIdentifier;
	Identifier::Compiled::ConstPtr    d_identifier;
	CollisionSolver::ConstPtr         d_solver;

};

} // namespace priv
} // namespace myrmidon
} // namespace fort
