#include "Identifier.hpp"

#include "Ant.hpp"

using namespace fort::myrmidon::priv;

Identifier::Identifier()
	: d_continuous(false) {

}

Identifier::Ptr Identifier::Create() {
	return std::shared_ptr<Identifier>(new Identifier());
}

AntPtr Identifier::CreateAnt() {
	auto res = std::make_shared<Ant>(NextAvailableID());
	d_ants[res->ID()] =  res;
	d_antIDs.insert(res->ID());
	return res;
}

void Identifier::DeleteAnt(fort::myrmidon::Ant::ID id) {
	if ( d_ants.count(id) == 0 ) {
		std::ostringstream os;
		os << "Could not find ant " << id ;
		throw std::out_of_range(os.str());
	}
	if ( id != d_ants.size() ) {
		d_continuous = false;
	}
	d_antIDs.erase(id);
	d_ants.erase(id);
}

const AntByID & Identifier::Ants() const {
	return d_ants;
}


fort::myrmidon::Ant::ID Identifier::NextAvailableID() {
	if ( d_continuous == true ) {
		return d_ants.size() + 1;
	}
	fort::myrmidon::Ant::ID res = 0;
	auto missingIndex = std::find_if(d_antIDs.begin(),d_antIDs.end(),[&res] (fort::myrmidon::Ant::ID toTest ) {
		                                                                 return ++res != toTest;
	                                                                 });

	if (missingIndex == d_antIDs.end() ) {
		const_cast<Identifier*>(this)->d_continuous = true;
		return d_antIDs.size() + 1;
	}
	return res;
}



void Identifier::LoadAnt(const Identifier::Ptr & itself,
                         const fort::myrmidon::pb::AntMetadata & pb) {
	fort::myrmidon::Ant::ID id = pb.id();
	itself->d_antIDs.insert(id);
	itself->d_ants[id] = Ant::FromSaved(pb,itself);
}
