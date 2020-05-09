#include "Space.hpp"

#include "priv/Space.hpp"

namespace fort {
namespace myrmidon {

Space::Space(const PPtr & pSpace)
	: d_p(pSpace) {
}

Space::ID Space::SpaceID() const {
	return d_p->SpaceID();
}

const std::string & Space::Name() const {
	return d_p->Name();
}

void Space::SetName(const std::string & name) const {
	d_p->SetName(name);
}

Zone::Ptr Space::CreateZone(const std::string & name) {
	return std::make_shared<Zone>(d_p->CreateZone(name,0));
}

void Space::DeleteZone(Zone::ID ID) {
	d_p->DeleteZone(ID);
}

Zone::ByID Space::Zones() {
	Zone::ByID res;
	for ( const auto & [zID,zone] : d_p->Zones() ) {
		res.insert(std::make_pair(zID,std::make_shared<Zone>(zone)));
	}
	return res;
}

Zone::ConstByID Space::CZones() const {
	Zone::ConstByID res;
	for ( const auto & [zID,zone] : d_p->Zones() ) {
		res.insert(std::make_pair(zID,std::make_shared<const Zone>(zone)));
	}
	return res;
}





} // namespace myrmidon
} // namespace fort
