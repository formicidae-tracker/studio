#include "Space.hpp"

#include "TrackingDataDirectory.hpp"

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

Space::Ptr Space::Universe::Create(const Ptr & itself, const std::string & name) {
	std::shared_ptr<Space> res(new Space(name,itself));
	itself->d_spaces.push_back(res);
	return res;
}

void Space::Universe::DeleteSpace(const std::string & URI) {
	auto fi = std::find_if(d_spaces.begin(),
	                       d_spaces.end(),
	                       [URI](const Space::Ptr & z) -> bool {
		                       return z->URI() == URI;
	                       });
	if ( fi == d_spaces.end() ) {
		throw UnmanagedSpace(URI);
	}

	if ( (*fi)->d_tdds.empty() == false ) {
		throw SpaceNotEmpty(**fi);
	}

	d_spaces.erase(fi);
}

void Space::Universe::DeleteTrackingDataDirectory(const std::string & URI) {
	for ( const auto & z : d_spaces ) {
		try {
			z->DeleteTrackingDataDirectory(URI);
			return;
		} catch ( const UnmanagedTrackingDataDirectory & e) {
		}
	}
	throw UnmanagedTrackingDataDirectory(URI);
}

const std::vector<Space::Ptr> & Space::Universe::Spaces() const {
	return d_spaces;
}

Space::Space(const std::string & name, const Universe::Ptr & universe)
	: d_universe(universe)
	, d_continuous(false) {
	SetName(name);
}

const Space::Universe::TrackingDataDirectoryByURI &
Space::Universe::TrackingDataDirectories() const {
	return d_tddsByURI;
}

bool HasPrefix(const std::string & s, const std::string & prefix ) {
	if (prefix.size() > s.size() ) {
		return false;
	}
	return std::mismatch(prefix.begin(),prefix.end(),s.begin(),s.end()).first == prefix.end();
}

void Space::AddTrackingDataDirectory(const TrackingDataDirectory::ConstPtr & tdd) {
	if ( HasPrefix(tdd->URI(),"spaces/") == true ) {
		throw std::runtime_error("Invalid TDD path '" + tdd->URI() + "': starts with 'spaces/'");
	}

	auto newList = d_tdds;
	newList.push_back(tdd);
	auto fi = TimeValid::SortAndCheckOverlap(newList.begin(),newList.end());
	if ( fi.first != fi.second ) {
		throw TDDOverlap(*fi.first,*fi.second);
	}

	auto universe = LockUniverse();

	auto zi = std::find_if(universe->d_spaces.begin(),
	                       universe->d_spaces.end(),
	                       [&tdd](const Space::Ptr & z) {
		                       return z->URI() == tdd->URI();
	                       });

	if ( zi != universe->d_spaces.end() ) {
		throw std::runtime_error("TDD:'"
		                         + tdd->URI()
		                         + " has the same than a Space in this universe");
	}

	if ( universe->d_tddsByURI.count(tdd->URI()) != 0 ) {
		auto zi = std::find_if(universe->d_spaces.begin(),
		                       universe->d_spaces.end(),
		                       [&tdd](const  Space::Ptr  &z) {
			                       auto ti = std::find_if(z->d_tdds.begin(),
			                                              z->d_tdds.end(),
			                                              [&tdd](const TrackingDataDirectory::ConstPtr & tdd) {
				                                              return tdd->URI() == tdd->URI();
			                                              });
			                       return ti != z->d_tdds.end();
		                       });
		if ( zi == universe->d_spaces.end()){
			throw std::logic_error("Internal data error");
		}

		throw TDDAlreadyInUse(tdd->URI(),(*zi)->URI());
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

void Space::SetName(const std::string & name) {
	fs::path URI = "spaces" / fs::path(name);
	if (name.empty()) {
		throw InvalidName(name,"it is empty");
	}
	if (fs::path(name).filename() != fs::path(name)) {
		throw InvalidName(name,"invalid character in path");
	}

	auto universe = LockUniverse();


	auto zi = std::find_if(universe->d_spaces.begin(),
	                       universe->d_spaces.end(),
	                       [URI](const Space::Ptr & z ) {
		                       return z->URI() == URI.generic_string();
	                       });
	if (zi != universe->d_spaces.end()) {
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
Space::Universe::LocateTrackingDataDirectory(const std::string & tddURI) const {
	auto tddi = d_tddsByURI.find(tddURI) ;
	if ( tddi == d_tddsByURI.end() ) {
		return std::make_pair(Space::Ptr(),TrackingDataDirectory::ConstPtr());
	}

	auto zi = std::find_if(d_spaces.begin(),
	                       d_spaces.end(),
	                       [&tddURI]( const Space::Ptr & z) {
		                       auto ti = std::find_if(z->d_tdds.begin(),
		                                              z->d_tdds.end(),
		                                              [&tddURI]( const TrackingDataDirectory::ConstPtr & tdd) {
			                                              return tdd->URI() == tddURI;
		                                              });
		                       return ti != z->d_tdds.end();
	                       });
	if ( zi ==  d_spaces.end()) {
		return std::make_pair(Space::Ptr(),TrackingDataDirectory::ConstPtr());
	}

	return std::make_pair(*zi,tddi->second);
}

Space::Ptr Space::Universe::LocateSpace(const std::string & spaceURI) const {
	auto zi = std::find_if(d_spaces.begin(),
	                       d_spaces.end(),
	                       [&spaceURI] ( const Space::Ptr & z) {
		                       return z->URI() == spaceURI;
	                       });
	if ( zi == d_spaces.end()) {
		return Space::Ptr();
	}
	return *zi;
}

Zone::ID Space::NextAvailableZoneID() {
	if ( d_continuous == true ) {
		return d_zones.size() + 1;
	}
	Zone::ID res = 0;
	auto missingIndex = std::find_if(d_zoneIDs.begin(),d_zoneIDs.end(),
	                                 [&res] ( const Zone::ID toTest ) {
		                                 return ++res != toTest;
	                                 });
	if ( missingIndex == d_zoneIDs.end() ) {
		d_continuous = true;
		return d_zones.size() + 1;
	}
	return res;
}

Zone::Ptr Space::CreateZone(Zone::ID ID) {
	if ( ID == NEXT_AVAILABLE_ID ) {
		ID =  NextAvailableZoneID();
	}
	if ( d_zoneIDs.count(ID) != 0 ) {
		throw std::invalid_argument("Zone " + std::to_string(ID) + " already exits");
	}
	auto res = Zone::Create(ID,"new-zone",d_URI);
	d_zones[ID] = res;
	d_zoneIDs.insert(ID);
	return res;
}

void Space::DeleteZone(Zone::ID ID) {
	auto fi = d_zones.find(ID);
	if ( fi == d_zones.end() ) {
		throw std::invalid_argument("Zone " +std::to_string(ID) + " does not exist");
	}
	if ( ID != d_zones.size() ) {
		d_continuous = false;
	}

	d_zones.erase(fi);
	d_zoneIDs.erase(ID);
}

const Space::ZoneByID & Space::Zones() const {
	return d_zones;
}


} //namespace priv
} //namespace myrmidon
} //namespace fort
