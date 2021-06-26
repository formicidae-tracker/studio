#pragma once

#include "ForwardDeclaration.hpp"
#include "Types.hpp"

#include "Ant.hpp"
#include "Zone.hpp"
#include "TimeMap.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class AntZoner {
public:
	typedef std::shared_ptr<AntZoner>       Ptr;
	typedef std::shared_ptr<const AntZoner> ConstPtr;
	typedef std::vector<std::pair<ZoneID,Zone::Geometry::ConstPtr> > ZoneGeometries;

	AntZoner(const ZoneGeometries & zoneGeometries);

	ZoneID LocateAnt(PositionedAntRef antRow) const;
private:
	ZoneGeometries d_zoneGeometries;
};


class CollisionSolver {
public:
	typedef std::shared_ptr<CollisionSolver>       Ptr;
	typedef std::shared_ptr<const CollisionSolver> ConstPtr;

	CollisionSolver(const SpaceByID & spaces,
	                const AntByID & ants);

	AntZoner::ConstPtr ZonerFor(const IdentifiedFrame & frame) const;

	CollisionFrame::Ptr
	ComputeCollisions(const IdentifiedFrame::Ptr & frame) const;
private:
	typedef DenseMap<AntID,TypedCapsuleList>                                AntGeometriesByID;
	typedef TimeMap<ZoneID,ZoneGeometry::ConstPtr>                          ZoneGeometriesByTime;
	typedef DenseMap<SpaceID,ZoneGeometriesByTime>                          GeometriesBySpaceID;
	typedef DenseMap<SpaceID,std::vector<ZoneID>>                           ZoneIDsBySpaceID;
	typedef std::unordered_map<ZoneID,std::vector<PositionedAntConstRef>>   LocatedAnts;

	void LocateAnts(LocatedAnts & locatedAnts,
	                const IdentifiedFrame::Ptr & frame) const;

	void ComputeCollisions(std::vector<Collision> &  result,
	                       const std::vector<PositionedAntConstRef> & positions,
	                       ZoneID zoneID) const;

	AntGeometriesByID    d_antGeometries;
	GeometriesBySpaceID  d_spaceDefinitions;
	ZoneIDsBySpaceID     d_zoneIDs;

};


} // namespace priv
} // namespace myrmidon
} // namespace fort
