#include "Space.hpp"

#include "priv/Space.hpp"
#include "priv/TrackingDataDirectory.hpp"

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {

Space::Space(const PPtr & pSpace)
	: d_p(pSpace) {
}

fort::myrmidon::SpaceID Space::SpaceID() const {
	return FORT_MYRMIDON_CONST_HELPER(Space,SpaceID);
}

Space::ID CSpace::SpaceID() const {
	return d_p->SpaceID();
}

const std::string & Space::Name() const {
	return FORT_MYRMIDON_CONST_HELPER(Space,Name);
}

const std::string & CSpace::Name() const {
	return d_p->Name();
}

void Space::SetName(const std::string & name) {
	d_p->SetName(name);
}

Zone Space::CreateZone(const std::string & name) {
	return Zone(d_p->CreateZone(name,0));
}

void Space::DeleteZone(Zone::ID ID) {
	d_p->DeleteZone(ID);
}

Zone::ByID Space::Zones() {
	Zone::ByID res;
	for ( const auto & [zID,zone] : d_p->Zones() ) {
		res.insert(std::make_pair(zID,Zone(zone)));
	}
	return res;
}

Zone::ConstByID Space::CZones() const {
	return FORT_MYRMIDON_CONST_HELPER(Space,CZones);
}

Zone::ConstByID CSpace::CZones() const {
	Zone::ConstByID res;
	for ( const auto & [zID,zone] : d_p->CZones() ) {
		res.insert(std::make_pair(zID,CZone(zone)));
	}
	return res;
}

std::pair<std::string,uint64_t> Space::LocateMovieFrame(const Time & time) const {
	return FORT_MYRMIDON_CONST_HELPER(Space,LocateMovieFrame,time);
}

std::pair<std::string,uint64_t> CSpace::LocateMovieFrame(const Time & time) const {
	for ( const auto & tdd : d_p->TrackingDataDirectories() ) {
		if ( tdd->IsValid(time) == false ) {
			continue;
		}

		auto ref = tdd->FrameReferenceAfter(time);
		auto movieSegment = tdd->MovieSegments().Find(time);

		auto movieFrameID = movieSegment.second->ToMovieFrameID(ref.FrameID());
		return std::make_pair(movieSegment.second->AbsoluteFilePath().string(),movieFrameID);
	}
	std::ostringstream oss;
	oss << "Could not find time " << time << " in space " << d_p->Name();
	throw std::runtime_error(oss.str());
}

CSpace::CSpace(const ConstPPtr & pSpace) :
	d_p(pSpace) {
}



} // namespace myrmidon
} // namespace fort
