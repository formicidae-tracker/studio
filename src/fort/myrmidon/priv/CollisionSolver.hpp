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

	ZoneID LocateAnt(const PositionedAnt & ant) const;
private:
	ZoneGeometries d_zoneGeometries;
};


class CollisionSolver {
public:
	typedef std::shared_ptr<CollisionSolver>       Ptr;
	typedef std::shared_ptr<const CollisionSolver> ConstPtr;

	CollisionSolver(const SpaceByID & spaces,
	                const AntByID & ants);

	AntZoner::ConstPtr ZonerFor(const IdentifiedFrame::ConstPtr & frame) const;

	CollisionFrame::ConstPtr
	ComputeCollisions(const IdentifiedFrame::Ptr & frame) const;
private:
	typedef DenseMap<AntID,Ant::TypedCapsuleList>                    AntGeometriesByID;
	typedef TimeMap<ZoneID,ZoneDefinition::ConstPtr>                 ZoneDefinitionsByTime;
	typedef DenseMap<SpaceID,ZoneDefinitionsByTime>                  DefinitionsBySpaceID;
	typedef DenseMap<SpaceID,std::vector<ZoneID>>                    ZoneIDsBySpaceID;
	typedef std::unordered_map<Zone::ID,std::vector<PositionedAnt> > LocatedAnts;

	void LocateAnts(LocatedAnts & locatedAnts,
	                const IdentifiedFrame::Ptr & frame) const;

	void ComputeCollisions(std::vector<Collision> &  result,
	                       const std::vector<PositionedAnt> & ants,
	                       ZoneID zoneID) const;

	AntGeometriesByID    d_antGeometries;
	DefinitionsBySpaceID d_spaceDefinitions;
	ZoneIDsBySpaceID     d_zoneIDs;

};


} // namespace priv
} // namespace myrmidon
} // namespace fort
