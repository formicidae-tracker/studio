#include "Space.hpp"

#include "priv/Space.hpp"
#include "priv/TrackingDataDirectory.hpp"

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {

Space::Space(const PPtr & pSpace)
	: d_p(pSpace) {
}

SpaceID Space::ID() const {
	return d_p->ID();
}

const std::string & Space::Name() const {
	return d_p->Name();
}

void Space::SetName(const std::string & name) {
	d_p->SetName(name);
}

Zone & Space::CreateZone(const std::string & name) {
	return d_p->PublicCreateZone(name,0);
}

void Space::DeleteZone(ZoneID zoneID) {
	d_p->DeleteZone(zoneID);
}

const ZoneByID & Space::Zones() const {
	return d_p->PublicZones();
}

std::pair<std::string,uint64_t> Space::LocateMovieFrame(const Time & time) const {
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



} // namespace myrmidon
} // namespace fort
