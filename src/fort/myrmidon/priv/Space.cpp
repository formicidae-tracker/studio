#include "Space.hpp"

#include "TrackingDataDirectory.hpp"
#include <fort/myrmidon/utils/StringManipulation.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

Space::TDDOverlap::TDDOverlap(const TrackingDataDirectory::ConstPtr & a,
                             const TrackingDataDirectory::ConstPtr & b) noexcept
	: std::runtime_error(BuildWhat(a,b))
	, d_a(a)
	, d_b(b) {
}

const TrackingDataDirectoryConstPtr & Space::TDDOverlap::A() const {
	return d_a;
}

const TrackingDataDirectoryConstPtr & Space::TDDOverlap::B() const {
	return d_b;
}

std::string Space::TDDOverlap::BuildWhat(const TrackingDataDirectory::ConstPtr & a,
                                        const TrackingDataDirectory::ConstPtr & b) noexcept {
	std::ostringstream oss;

	oss << *a << " and " << *b << " overlaps in time";

	return oss.str();
}

Space::UnmanagedTrackingDataDirectory::UnmanagedTrackingDataDirectory(const std::string & URI) noexcept
	: std::runtime_error("TDD:'" + URI + "' is not managed by this Space or Universe") {
}

Space::UnmanagedSpace::UnmanagedSpace(const std::string & URI) noexcept
	: std::runtime_error("Space:'" + URI + "' is not managed by this Universe") {
}

Space::InvalidName::InvalidName(const std::string & name,
                                const std::string & reason) noexcept
	: std::runtime_error("Invalid Space name '" + name + "': " + reason) {
}

Space::SpaceNotEmpty::SpaceNotEmpty(const Space & z)
	: std::runtime_error(BuildReason(z)) {
}

std::string Space::SpaceNotEmpty::BuildReason(const Space & z) {
	std::ostringstream oss;
	oss << "Space:'" << z.Name()
	    << "' is not empty (contains:";
	std::string sep = "{";
	for ( const auto & tdd : z.d_tdds ) {
		oss << sep << tdd->URI();
		sep = ",";
	}
	oss << "})";
	return oss.str();
}

Space::TDDAlreadyInUse::TDDAlreadyInUse(const std::string & tddURI, const std::string & spaceURI)
	: std::runtime_error("TDD:'"
	                     + tddURI
	                     + "' is in use in Space:'"
	                     + spaceURI + "'") {
}

Space::Ptr Space::Universe::Create(const Ptr & itself,
                                   Space::ID spaceID,
                                   const std::string & name) {
	return itself->d_spaces.CreateObject([&itself,&name](Space::ID ID) {
		                                     return Space::Ptr(new Space(ID,name,itself));
	                                    });
}

void Space::Universe::DeleteSpace(Space::ID ID) {
	auto fi = d_spaces.Objects().find(ID);
	if ( fi == d_spaces.Objects().end() ) {
		throw UnmanagedSpace("spaces/" + std::to_string(ID));
	}

	if ( fi->second->d_tdds.empty() == false ) {
		throw SpaceNotEmpty(*fi->second);
	}

	d_spaces.DeleteObject(ID);
}

void Space::Universe::DeleteTrackingDataDirectory(const std::string & URI) {
	for ( const auto & [spaceID,s] : d_spaces.Objects() ) {
		try {
			s->DeleteTrackingDataDirectory(URI);
			return;
		} catch ( const UnmanagedTrackingDataDirectory & e) {
		}
	}
	throw UnmanagedTrackingDataDirectory(URI);
}

const ConstSpaceByID & Space::Universe::CSpaces() const {
	return d_spaces.CObjects();
}

const SpaceByID & Space::Universe::Spaces() {
	return d_spaces.Objects();
}

Space::Space(ID spaceID, const std::string & name, const Universe::Ptr & universe)
	: d_universe(universe)
	, d_ID(spaceID) {
	SetName(name);
}

const Space::Universe::TrackingDataDirectoryByURI &
Space::Universe::TrackingDataDirectories() const {
	return d_tddsByURI;
}


void Space::AddTrackingDataDirectory(const TrackingDataDirectory::ConstPtr & tdd) {
	if ( utils::HasPrefix(tdd->URI(),"spaces/") == true ) {
		throw std::runtime_error("Invalid TDD path '" + tdd->URI() + "': starts with 'spaces/'");
	}

	auto newList = d_tdds;
	newList.push_back(tdd);
	auto fi = TimeValid::SortAndCheckOverlap(newList.begin(),newList.end());
	if ( fi.first != fi.second ) {
		throw TDDOverlap(*fi.first,*fi.second);
	}

	auto universe = LockUniverse();

	if ( universe->d_tddsByURI.count(tdd->URI()) != 0 ) {
		auto si = std::find_if(universe->d_spaces.Objects().begin(),
		                       universe->d_spaces.Objects().end(),
		                       [&tdd](const std::pair<Space::ID,Space::Ptr> & iter) {
			                       auto ti = std::find_if(iter.second->d_tdds.begin(),
			                                              iter.second->d_tdds.end(),
			                                              [&tdd](const TrackingDataDirectory::ConstPtr & tddb) {
				                                              return tdd->URI() == tddb->URI();
			                                              });
			                       return ti != iter.second->d_tdds.end();
		                       });
		if ( si == universe->d_spaces.Objects().end()){
			throw std::logic_error("Internal data error");
		}

		throw TDDAlreadyInUse(tdd->URI(),si->second->URI());
	}

	d_tdds = newList;
	universe->d_tddsByURI.insert(std::make_pair(tdd->URI(),tdd));
}

void Space::DeleteTrackingDataDirectory(const std::string & URI) {
	auto fi = std::find_if(d_tdds.begin(),
	                       d_tdds.end(),
	                       [&URI](const TrackingDataDirectory::ConstPtr & tdd) {
		                       return URI == tdd->URI();
	                       });
	if ( fi == d_tdds.end() ) {
		throw UnmanagedTrackingDataDirectory(URI);
	}
	auto universe = LockUniverse();
	universe->d_tddsByURI.erase(URI);

	d_tdds.erase(fi);
}

const std::string & Space::URI() const {
	return d_URI;
}

const std::string & Space::Name() const {
	return d_name;
}

Space::ID Space::SpaceID() const {
	return d_ID;
}


void Space::SetName(const std::string & name) {
	fs::path URI = fs::path("spaces") / std::to_string(d_ID);
	if (name.empty()) {
		throw InvalidName(name,"it is empty");
	}
	if (fs::path(name).filename() != fs::path(name)) {
		throw InvalidName(name,"invalid character in path");
	}

	auto universe = LockUniverse();

	auto zi = std::find_if(universe->d_spaces.Objects().begin(),
	                       universe->d_spaces.Objects().end(),
	                       [&name](const std::pair<Space::ID,Space::Ptr> & iter ) {
		                       return iter.second->Name() == name;
	                       });
	if (zi != universe->d_spaces.Objects().end()) {
		throw InvalidName(name,"is already used by another Space");
	}

	if ( universe->d_tddsByURI.count(name) != 0 ) {
		throw InvalidName(name,"is already used by another TDD");
	}

	d_name = name;
	d_URI = URI.generic_string();
}

const std::vector<TrackingDataDirectory::ConstPtr> & Space::TrackingDataDirectories() const {
	return d_tdds;
}

Space::Universe::Ptr Space::LockUniverse() const {
	auto locked = d_universe.lock();
	if ( !locked ) {
		throw DeletedReference<Space::Universe>();
	}
	return locked;
}

std::pair<Space::Ptr,TrackingDataDirectory::ConstPtr>
Space::Universe::LocateTrackingDataDirectory(const std::string & tddURI) {
	auto res = CLocateTrackingDataDirectory(tddURI);
	return std::make_pair(std::const_pointer_cast<Space>(res.first),
	                      res.second);
}

std::pair<Space::ConstPtr,TrackingDataDirectory::ConstPtr>
Space::Universe::CLocateTrackingDataDirectory(const std::string & tddURI) const {
	auto tddi = d_tddsByURI.find(tddURI) ;
	if ( tddi == d_tddsByURI.end() ) {
		return std::make_pair(Space::Ptr(),TrackingDataDirectory::ConstPtr());
	}

	auto si = std::find_if(d_spaces.CObjects().begin(),
	                       d_spaces.CObjects().end(),
	                       [&tddURI]( const std::pair<Space::ID,Space::ConstPtr> & iter) {
		                       auto ti = std::find_if(iter.second->d_tdds.begin(),
		                                              iter.second->d_tdds.end(),
		                                              [&tddURI]( const TrackingDataDirectory::ConstPtr & tdd) {
			                                              return tdd->URI() == tddURI;
		                                              });
		                       return ti != iter.second->d_tdds.end();
	                       });
	if ( si ==  d_spaces.CObjects().end()) {
		return std::make_pair(Space::Ptr(),TrackingDataDirectory::ConstPtr());
	}

	return std::make_pair(si->second,tddi->second);
}

Space::Ptr Space::Universe::LocateSpace(const std::string & spaceName) {
	return std::const_pointer_cast<Space>(CLocateSpace(spaceName));
}

Space::ConstPtr Space::Universe::CLocateSpace(const std::string & spaceName) const {
	auto si = std::find_if(d_spaces.CObjects().begin(),
	                       d_spaces.CObjects().end(),
	                       [&spaceName] ( const std::pair<Space::ID,Space::ConstPtr> & iter) {
		                       return iter.second->Name() == spaceName;
	                       });
	if ( si == d_spaces.CObjects().end()) {
		return Space::ConstPtr();
	}
	return si->second;
}

Zone::Ptr Space::CreateZone(const std::string & name, Zone::ID ID) {
	return d_zones.CreateObject([&name,this](Zone::ID ZID) { return Zone::Create(ZID,name,d_URI); },ID);
}

void Space::DeleteZone(Zone::ID ID) {
	d_zones.DeleteObject(ID);
}

const ConstZoneByID & Space::CZones() const {
	return d_zones.CObjects();
}

const ZoneByID & Space::Zones() {
	return d_zones.Objects();
}



} //namespace priv
} //namespace myrmidon
} //namespace fort
