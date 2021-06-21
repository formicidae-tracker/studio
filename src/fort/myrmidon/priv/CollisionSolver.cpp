#include "CollisionSolver.hpp"

#include "Space.hpp"
#include <fort/myrmidon/Shapes.hpp>
#include "AntShapeType.hpp"
#include "KDTree.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

CollisionSolver::CollisionSolver(const SpaceByID & spaces,
                                 const AntByID & ants) {

	// Deep copy ant shape data.
	for ( const auto & [aID,ant] : ants ) {
		d_antGeometries.insert(std::make_pair(aID,ant->Capsules()));
	}

	for ( const auto & [spaceID,space] : spaces ) {
		auto res = d_spaceDefinitions.insert(std::make_pair(spaceID,ZoneGeometriesByTime()));
		d_zoneIDs.insert(std::make_pair(spaceID,std::vector<ZoneID>()));
		auto & definitions = res.first->second;
		for ( const auto & [zID,zone] : space->Zones() ) {
			d_zoneIDs.at(spaceID).push_back(zID);
			for ( const auto & definition : zone->Definitions() ) {
				definitions.InsertOrAssign(zID,
				                           std::make_shared<ZoneGeometry>(definition->Shapes()),
				                           definition->Start());
				if ( definition->End().IsForever() == false) {
					definitions.InsertOrAssign(zID,
					                           std::make_shared<ZoneGeometry>(Shape::List()),
					                           definition->End());
				}
			}
		}
	}
}

CollisionFrame::Ptr
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


AntZoner::ConstPtr CollisionSolver::ZonerFor(const IdentifiedFrame & frame) const {
	if ( d_spaceDefinitions.count(frame.Space) == 0) {
		throw std::invalid_argument("Unknown SpaceID " + std::to_string(frame.Space) + " in IdentifiedFrame");
	}
	const auto & allDefinitions = d_spaceDefinitions.at(frame.Space);

	// first we build geometries for the right time;
	std::vector<std::pair<ZoneID,Zone::Geometry::ConstPtr> > currentGeometries;
	for ( const auto & zID : d_zoneIDs.at(frame.Space) ) {
		try {
			auto geometry = allDefinitions.At(zID,frame.FrameTime);
			currentGeometries.push_back(std::make_pair(zID,geometry));
		} catch ( const std::exception & e ) {
			continue;
		}
	}
	return std::make_shared<AntZoner>(currentGeometries);
}


AntZoner::AntZoner(const ZoneGeometries & zoneGeometries)
	: d_zoneGeometries(zoneGeometries) {
}

ZoneID AntZoner::LocateAnt(PositionedAntRef ant) const {
	auto fi =  std::find_if(d_zoneGeometries.begin(),
	                        d_zoneGeometries.end(),
	                        [&ant](const std::pair<ZoneID,Zone::Geometry::ConstPtr> & iter ) -> bool {
		                        return iter.second->Contains(ant.block<1,2>(0,1).transpose());
	                        });
	if ( fi == d_zoneGeometries.end() ) {
		return 0;
	}
	ant(0,4) = fi->first;
	return fi->first;
}


void CollisionSolver::LocateAnts(LocatedAnts & locatedAnts,
                                 const IdentifiedFrame::Ptr & frame) const {

	auto zoner = ZonerFor(*frame);

	// now for each geometry. we test if the ants is in the zone
	for ( size_t i = 0; i < frame->Positions.rows(); ++i ) {
		ZoneID zoneID = zoner->LocateAnt(frame->Positions.row(i));
		locatedAnts[zoneID].push_back(frame->Positions.row(i));
	}
}


void CollisionSolver::ComputeCollisions(std::vector<Collision> &  result,
                                        const std::vector<PositionedAntConstRef> & ants,
                                        ZoneID zoneID) const {

	//first-pass we compute possible interactions
	struct AntTypedCapsule  {
		Capsule          C;
		AntID            ID;
		AntShapeType::ID TypeID;
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
		AntID antID = ant(0,0);
		auto fiGeom = d_antGeometries.find(antID);
		if ( fiGeom == d_antGeometries.end() ) {
			continue;
		}
		Isometry2Dd antToOrig(ant(0,3),ant.block<1,2>(0,1).transpose());

		for ( const auto & [typeID,c] : fiGeom->second ) {
			auto data =
				AntTypedCapsule { .C = c->Transform(antToOrig),
				                  .ID = antID,
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
