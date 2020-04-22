#include "InteractionSolver.hpp"

#include "Space.hpp"
#include "Capsule.hpp"
#include "AntShapeType.hpp"
#include "KDTree.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

InteractionSolver::InteractionSolver(const SpaceByID & spaces,
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

InteractionFrame::ConstPtr InteractionSolver::ComputeInteraction(SpaceID spaceID,
                                                                 const IdentifiedFrame::ConstPtr & frame) const {
	LocatedAnts locatedAnts;
	LocateAnts(locatedAnts,spaceID,frame);
	auto res = std::make_shared<InteractionFrame>();
	res->FrameTime = frame->FrameTime;
	for ( const auto & [zID,ants] : locatedAnts ) {
		ComputeInteraction(res->Interactions,ants);
	}
	return res;
}

void InteractionSolver::LocateAnts(LocatedAnts & locatedAnts,
                                   SpaceID spaceID,
                                   const IdentifiedFrame::ConstPtr & frame) const {

	if ( d_spaceGeometries.count(spaceID) == 0) {
		throw std::invalid_argument("Invalid space " + std::to_string(spaceID));
	}
	const auto & allGeometries = d_spaceGeometries.at(spaceID);

	// first we build geometries for the right time;
	std::vector<std::pair<ZoneID,Zone::Geometry::ConstPtr> > currentGeometries;
	for ( const auto & zID : d_zoneIDs.at(spaceID) ) {
		try {
			currentGeometries.push_back(std::make_pair(zID,allGeometries.At(zID,frame->FrameTime)));
		} catch ( const std::exception & e ) {
			continue;
		}
	}

	// now for each geometry. we test if the ants is in the zone
	for ( const auto & p : frame->Positions ) {
		bool found = false;
		for ( const auto & [zID,geometry] : currentGeometries ) {
			if (geometry->Contains(p.Position) == true ) {
				locatedAnts[zID].push_back(p);
				found = true;
				break;
			}
		}
		if ( found == false ) {
			locatedAnts[0].push_back(p);
		}
	}
}


void InteractionSolver::ComputeInteraction(std::vector<Interaction> &  result,
                                           const std::vector<PositionedAnt> & ants) const {

	//first-pass we compute possible interactions
	struct AntTypedCapsule  {
		Capsule::ConstPtr       C;
		fort::myrmidon::Ant::ID AntID;
		AntShapeType::ID        TypeID;
		inline bool operator<( const AntTypedCapsule & other ) {
			return AntID < other.AntID;
		}
		inline bool operator>( const AntTypedCapsule & other ) {
			return AntID > other.AntID;
		}
		inline bool operator!=( const AntTypedCapsule & other ) {
			return AntID != other.AntID;
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
				AntTypedCapsule { .C = std::make_shared<Capsule>(c->Transform(antToOrig)),
				                  .AntID = uint32_t(ant.ID),
				                  .TypeID = typeID,
			};
			nodes.push_back({.Object = data, .Volume = data.C->ComputeAABB() });
		}
	}
	auto kdt = KDT::Build(nodes.begin(),nodes.end(),-1);
	std::list<std::pair<AntTypedCapsule,AntTypedCapsule>> possibleCollisions;
	auto inserter = std::inserter(possibleCollisions,possibleCollisions.begin());
	kdt->ComputeCollisions(inserter);

	// now do the actual collisions
	std::map<InteractionID,std::vector<InteractionType> > res;
	for ( const auto & coarse : possibleCollisions ) {
		if ( coarse.first.C->Intersects(*coarse.second.C) == true ) {
			InteractionID ID = std::make_pair(coarse.first.AntID,coarse.second.AntID);
			InteractionType type = std::make_pair(coarse.first.TypeID,coarse.second.TypeID);
			res[ID].push_back(type);
		}
	}
	result.reserve(result.size() + res.size());
	for ( const auto & [ID,interactions] : res ) {
		result.push_back({ID,interactions});
	}

}


} // namespace priv
} // namespace myrmidon
} // namespace fort
