#pragma once

#include "ForwardDeclaration.hpp"
#include "Types.hpp"

#include "Ant.hpp"
#include "Zone.hpp"
#include "TimeMap.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class InteractionSolver {
public:
	typedef std::shared_ptr<InteractionSolver>       Ptr;
	typedef std::shared_ptr<const InteractionSolver> ConstPtr;

	InteractionSolver(const SpaceByID & spaces,
	                  const AntByID & ants);

	InteractionFrame::ConstPtr
	ComputeInteractions(const IdentifiedFrame::ConstPtr & frame) const;
private:
	typedef DenseMap<AntID,Ant::TypedCapsuleList>                    AntGeometriesByID;
	typedef TimeMap<ZoneID,Zone::Geometry::ConstPtr>                 TimedZoneGeometries;
	typedef DenseMap<SpaceID,TimedZoneGeometries>                    GeometriesBySpaceID;
	typedef DenseMap<SpaceID,std::vector<ZoneID>>                    ZoneIDsBySpaceID;
	typedef std::unordered_map<Zone::ID,std::vector<PositionedAnt> > LocatedAnts;

	void LocateAnts(LocatedAnts & locatedAnts,
	                const IdentifiedFrame::ConstPtr & frame) const;

	void ComputeInteractions(std::vector<PonctualInteraction> &  result,
	                         const std::vector<PositionedAnt> & ants,
	                         SpaceID spaceID,
	                         ZoneID zoneID) const;

	AntGeometriesByID   d_antGeometries;
	GeometriesBySpaceID d_spaceGeometries;
	ZoneIDsBySpaceID    d_zoneIDs;

};


} // namespace priv
} // namespace myrmidon
} // namespace fort
