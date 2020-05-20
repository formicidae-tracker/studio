#pragma once

#include "ForwardDeclaration.hpp"
#include "Types.hpp"

#include "Ant.hpp"
#include "Zone.hpp"
#include "TimeMap.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class CollisionSolver {
public:
	typedef std::shared_ptr<CollisionSolver>       Ptr;
	typedef std::shared_ptr<const CollisionSolver> ConstPtr;

	CollisionSolver(const SpaceByID & spaces,
	                  const AntByID & ants);

	CollisionFrame::ConstPtr
	ComputeCollisions(const IdentifiedFrame::ConstPtr & frame) const;
private:
	typedef DenseMap<AntID,Ant::TypedCapsuleList>                    AntGeometriesByID;
	typedef TimeMap<ZoneID,Zone::Geometry::ConstPtr>                 TimedZoneGeometries;
	typedef DenseMap<SpaceID,TimedZoneGeometries>                    GeometriesBySpaceID;
	typedef DenseMap<SpaceID,std::vector<ZoneID>>                    ZoneIDsBySpaceID;
	typedef std::unordered_map<Zone::ID,std::vector<PositionedAnt> > LocatedAnts;

	void LocateAnts(LocatedAnts & locatedAnts,
	                const IdentifiedFrame::ConstPtr & frame) const;

	void ComputeCollisions(std::vector<Collision> &  result,
	                       const std::vector<PositionedAnt> & ants,
	                       ZoneID zoneID) const;

	AntGeometriesByID   d_antGeometries;
	GeometriesBySpaceID d_spaceGeometries;
	ZoneIDsBySpaceID    d_zoneIDs;

};


} // namespace priv
} // namespace myrmidon
} // namespace fort
