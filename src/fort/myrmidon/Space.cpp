#include "Space.hpp"

#include "priv/Space.hpp"
#include "priv/TrackingDataDirectory.hpp"

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

std::pair<std::string,uint64_t> Space::LocateMovieFrame(const Time & time) const {
	for ( const auto & tdd : d_p->TrackingDataDirectories() ) {
		if ( tdd->IsValid(time) == false ) {
			continue;
		}

		auto ref = tdd->FrameReferenceAfter(time);
		auto movieSegment = tdd->MovieSegments().Find(time);

		auto movieFrameID = movieSegment.second->ToMovieFrameID(ref.FID());
		return std::make_pair(movieSegment.second->AbsoluteFilePath().string(),movieFrameID);
	}
	std::ostringstream oss;
	oss << "Could not find time " << time << " in space " << d_p->Name();
	throw std::runtime_error(oss.str());
}





} // namespace myrmidon
} // namespace fort
