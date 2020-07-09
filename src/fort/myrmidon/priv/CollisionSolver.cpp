#include "CollisionSolver.hpp"

#include "Space.hpp"
#include "Capsule.hpp"
#include "AntShapeType.hpp"
#include "KDTree.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

CollisionSolver::CollisionSolver(const SpaceByID & spaces,
                                 const AntByID & ants) {

	for ( const auto & [aID,ant] : ants ) {
		d_antGeometries.insert(std::make_pair(aID,ant->Capsules()));
	}

	for ( const auto & [spaceID,space] : spaces ) {
		auto res = d_spaceGeometries.insert(std::make_pair(spaceID,TimedZoneGeometries()));
		d_zoneIDs.insert(std::make_pair(spaceID,std::vector<ZoneID>()));
		auto & geometries = res.first->second;
		for ( const auto & [zID,zone] : space->Zones() ) {
			d_zoneIDs.at(spaceID).push_back(zID);
			Time::ConstPtr last;
			for ( const auto & definition : zone->Definitions() ) {
				if ( Time::SortKey(definition->Start()) != Time::SortKey(last) ) {
					geometries.Insert(zID,{},last);
				}
				last = definition->End();
				geometries.Insert(zID,definition->GetGeometry(),definition->Start());
			}
			if ( !last ) {
				geometries.Insert(zID,{},last);
			}
		}
	}
}

CollisionFrame::ConstPtr
CollisionSolver::ComputeCollisions(const IdentifiedFrame::Ptr & frame) const {
	LocatedAnts locatedAnts;
	LocateAnts(locatedAnts,frame);
	auto res = std::make_shared<CollisionFrame>();
	res->FrameTime = frame->FrameTime;
	res->Space = frame->Space;
	for ( const auto & [zID,ants] : locatedAnts ) {
		ComputeCollisions(res->Collisions,ants,zID);
	}
	return res;
}


AntZoner::ConstPtr CollisionSolver::ZonerFor(const IdentifiedFrame::ConstPtr & frame) const {
	if ( d_spaceGeometries.count(frame->Space) == 0) {
		throw std::invalid_argument("Unknown SpaceID " + std::to_string(frame->Space) + " in IdentifiedFrame");
	}
	const auto & allGeometries = d_spaceGeometries.at(frame->Space);

	// first we build geometries for the right time;
	std::vector<std::pair<ZoneID,Zone::Geometry::ConstPtr> > currentGeometries;
	for ( const auto & zID : d_zoneIDs.at(frame->Space) ) {
		try {
			currentGeometries.push_back(std::make_pair(zID,allGeometries.At(zID,frame->FrameTime)));
		} catch ( const std::exception & e ) {
			continue;
		}
	}
	return std::make_shared<AntZoner>(currentGeometries);
}


AntZoner::AntZoner(const ZoneGeometries & zoneGeometries)
	: d_zoneGeometries(zoneGeometries) {
}

ZoneID AntZoner::LocateAnt(const PositionedAnt & ant) const {
	auto fi =  std::find_if(d_zoneGeometries.begin(),
	                        d_zoneGeometries.end(),
	                        [&ant](const std::pair<ZoneID,Zone::Geometry::ConstPtr> & iter ) -> bool {
		                        return iter.second->Contains(ant.Position);
	                        });
	if ( fi == d_zoneGeometries.end() ) {
		return 0;
	}
	return fi->first;
}


void CollisionSolver::LocateAnts(LocatedAnts & locatedAnts,
                                 const IdentifiedFrame::Ptr & frame) const {

	auto zoner = ZonerFor(frame);

	// now for each geometry. we test if the ants is in the zone
	frame->Zones.reserve(frame->Positions.size());
	for ( const auto & p : frame->Positions ) {
		auto zoneID = zoner->LocateAnt(p);
		locatedAnts[zoneID].push_back(p);
		frame->Zones.push_back(zoneID);
	}

}


void CollisionSolver::ComputeCollisions(std::vector<Collision> &  result,
                                        const std::vector<PositionedAnt> & ants,
                                        ZoneID zoneID) const {

	//first-pass we compute possible interactions
	struct AntTypedCapsule  {
		Capsule           C;
		AntID             ID;
		AntShapeType::ID  TypeID;
		inline bool operator<( const AntTypedCapsule & other ) {
			return ID < other.ID;
		}
		inline bool operator>( const AntTypedCapsule & other ) {
			return ID > other.ID;
		}
		inline bool operator!=( const AntTypedCapsule & other ) {
			return ID != other.ID;
		}
	};
	typedef KDTree<AntTypedCapsule,double,2> KDT;

	std::vector<KDT::Element> nodes;

	for ( const auto & ant : ants) {
		auto fiGeom = d_antGeometries.find(ant.ID);
		if ( fiGeom == d_antGeometries.end() ) {
			continue;
		}
		Isometry2Dd antToOrig(ant.Angle,ant.Position);

		for ( const auto & [typeID,c] : fiGeom->second ) {
			auto data =
				AntTypedCapsule { .C = c.Transform(antToOrig),
				                  .ID = uint32_t(ant.ID),
				                  .TypeID = typeID,
			};
			nodes.push_back({.Object = data, .Volume = data.C.ComputeAABB() });
		}
	}
	auto kdt = KDT::Build(nodes.begin(),nodes.end(),-1);
	std::list<std::pair<AntTypedCapsule,AntTypedCapsule>> possibleCollisions;
	auto inserter = std::inserter(possibleCollisions,possibleCollisions.begin());
	kdt->ComputeCollisions(inserter);

	// now do the actual collisions
	std::map<InteractionID,std::set<std::pair<uint32_t,uint32_t>>> res;
	for ( const auto & coarse : possibleCollisions ) {
		if ( coarse.first.C.Intersects(coarse.second.C) == true ) {
			InteractionID ID = std::make_pair(coarse.first.ID,coarse.second.ID);
			auto type = std::make_pair(coarse.first.TypeID,coarse.second.TypeID);
			res[ID].insert(type);
		}
	}
	result.reserve(result.size() + res.size());
	for ( const auto & [ID,interactionSet] : res ) {
		InteractionTypes interactions(interactionSet.size(),2);
		size_t i = 0;
		for ( const auto & t : interactionSet ) {
			interactions(i,0) = t.first;
			interactions(i,1) = t.second;
			++i;
		}
		result.push_back(Collision{ID,interactions,zoneID});
	}

}


} // namespace priv
} // namespace myrmidon
} // namespace fort
