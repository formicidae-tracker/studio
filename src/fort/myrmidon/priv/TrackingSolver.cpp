#include "TrackingSolver.hpp"


namespace fort {
namespace myrmidon {
namespace priv {


TrackingSolver::TrackingSolver(const Identifier::Compiled::ConstPtr & identifier,
                               const CollisionSolver::ConstPtr & solver)
	: d_identifier(identifier)
	, d_solver(solver) {
}

IdentifiedFrame::Ptr TrackingSolver::IdentifyFrame(const fort::hermes::FrameReadout & frame,
                                                   SpaceID spaceID) const {

	auto res = std::make_shared<IdentifiedFrame>();
	res->Space = spaceID;
	res->FrameTime = Time::FromTimestamp(frame.time());
	res->Width = frame.width();
	res->Height = frame.height();
	res->Positions.reserve(frame.tags().size());
	Eigen::Vector2d position;
	double angle;
	for ( const auto & t : frame.tags() ) {
		auto identification = d_identifier->Identify(t.id(),res->FrameTime);
		if ( !identification == true ) {
			continue;
		}
		identification->ComputePositionFromTag(position,angle,Eigen::Vector2d(t.x(),t.y()),t.theta());
		res->Positions.push_back({position,angle,identification->Target()->AntID()});
	}
	return res;
}

CollisionFrame::ConstPtr TrackingSolver::CollideFrame(const IdentifiedFrame::Ptr & identified) const {
	return d_solver->ComputeCollisions(identified);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
