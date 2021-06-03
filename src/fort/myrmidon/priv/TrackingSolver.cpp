#include "TrackingSolver.hpp"


namespace fort {
namespace myrmidon {
namespace priv {


TrackingSolver::TrackingSolver(const std::shared_ptr<const Identifier> & identifier,
                               const CollisionSolver::ConstPtr & solver)
	: d_rawIdentifier(identifier)
	, d_solver(solver) {
	d_identifier = d_rawIdentifier->Compile();
}

IdentifiedFrame::Ptr TrackingSolver::IdentifyFrame(const fort::hermes::FrameReadout & frame,
                                                   SpaceID spaceID) const {

	auto res = std::make_shared<IdentifiedFrame>();
	res->Space = spaceID;
	res->FrameTime = Time::FromTimestamp(frame.time());
	res->Width = frame.width();
	res->Height = frame.height();
	res->Positions.resize(frame.tags().size(),5);
	size_t index = 0;
	for ( const auto & t : frame.tags() ) {
		auto identification = d_identifier->Identify(t.id(),res->FrameTime);
		if ( !identification == true ) {
			continue;
		}
		res->Positions(index,0) = identification->Target()->AntID();
		res->Positions(index,4) = 0;
		identification->ComputePositionFromTag(res->Positions(index,1),
		                                       res->Positions(index,2),
		                                       res->Positions(index,3),
		                                       Eigen::Vector2d(t.x(),t.y()),
		                                       t.theta());
		++index;
	}
	res->Positions.conservativeResize(index,5);
	return res;
}

CollisionFrame::ConstPtr TrackingSolver::CollideFrame(const IdentifiedFrame::Ptr & identified) const {
	return d_solver->ComputeCollisions(identified);
}

AntID TrackingSolver::IdentifyTag(TagID tagID, const Time & time) {
	auto identification = d_identifier->Identify(tagID,time);
	if ( ! identification ) {
		return 0;
	}
	return identification->Target()->AntID();
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
