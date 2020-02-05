#include "Zone.hpp"

#include "TrackingDataDirectory.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

Zone::TDDOverlap::TDDOverlap(const TrackingDataDirectory::ConstPtr & a,
                             const TrackingDataDirectory::ConstPtr & b) noexcept
	: std::runtime_error(BuildWhat(a,b))
	, d_a(a)
	, d_b(b) {
}

const TrackingDataDirectoryConstPtr & Zone::TDDOverlap::A() const {
	return d_a;
}

const TrackingDataDirectoryConstPtr & Zone::TDDOverlap::B() const {
	return d_b;
}

std::string Zone::TDDOverlap::BuildWhat(const TrackingDataDirectory::ConstPtr & a,
                                        const TrackingDataDirectory::ConstPtr & b) noexcept {
	std::ostringstream oss;

	oss << "Path{URI:" << a->URI()
	    << ", start:" << a->StartDate()
	    << ", end:" << a->EndDate()
	    << "} and Path{URI:" << b->URI()
	    << ", start:" << b->StartDate()
	    << ", end:" << b->EndDate()
	    << "} overlaps in time";

	return oss.str();
}

Zone::UnmanagedTrackingDataDirectory::UnmanagedTrackingDataDirectory(const fs::path & URI) noexcept
	: std::runtime_error("'" + URI.generic_string() + "' is not managed by this zone") {
}

Zone::Manager::Manager() {}

Zone::Ptr Zone::Manager::Create(const Ptr & itself, const std::string & name) {
	std::shared_ptr<Zone> res(new Zone(name,itself));
	itself->d_zonesByURI.insert(std::make_pair(res->URI(),res));
	itself->d_zones.push_back(res);
	return res;
}

void Zone::Manager::DeleteZone(const fs::path & URI) {
	auto fi = d_zonesByURI.find(URI);
	if ( fi == d_zonesByURI.end() ) {
		//TODO custom exception ?
		throw std::invalid_argument("'" + URI.generic_string() + "' is not managed");
	}

	if ( fi->second->d_tdds.empty() == false ) {
		throw std::runtime_error("'" + URI.generic_string() + "' contains tracking data directories");
	}

	d_zonesByURI.erase(fi);
}

void Zone::Manager::DeleteTrackingDataDirectory(const fs::path & URI) {
	for ( const auto & z : d_zones ) {
		try {
			z->DeleteTrackingDataDirectory(URI);
			return;
		} catch ( const UnmanagedTrackingDataDirectory & e) {
		}
	}
	throw UnmanagedTrackingDataDirectory(URI);
}

const std::vector<Zone::Ptr> & Zone::Manager::Zones() const {
	return d_zones;
}

Zone::Zone(const std::string & name, const Manager::Ptr & manager)
	: d_manager(manager) {
	SetName(name);
}

const Zone::Manager::TrackingDataDirectoryByURI &
Zone::Manager::TrackingDataDirectories() const {
	return d_tddsByURI;
}


void Zone::AddTrackingDataDirectory(const TrackingDataDirectory::ConstPtr & tdd) {
	auto newList = d_tdds;
	newList.push_back(tdd);
	auto fi = TimeValid::SortAndCheckOverlap(newList.begin(),newList.end());
	if ( fi.first != fi.second ) {
		throw TDDOverlap(*fi.first,*fi.second);
	}

	auto manager = LockManager();
	if ( manager->d_tddsByURI.count(tdd->URI()) != 0 ) {
		throw std::runtime_error("'" + tdd->URI().generic_string() + "' is use in another zone");
	}

	d_tdds = newList;
	manager->d_tddsByURI.insert(std::make_pair(tdd->URI(),tdd));
}

void Zone::DeleteTrackingDataDirectory(const fs::path & URI) {
	auto fi = std::find_if(d_tdds.begin(),
	                       d_tdds.end(),
	                       [&URI](const TrackingDataDirectory::ConstPtr & tdd) {
		                       return URI == tdd->URI();
	                       });
	if ( fi == d_tdds.end() ) {
		throw UnmanagedTrackingDataDirectory(URI);
	}
	auto manager = LockManager();
	manager->d_tddsByURI.erase(URI);

	d_tdds.erase(fi);
}

const fs::path & Zone::URI() const {
	return d_URI;
}

void Zone::SetName(const std::string & name) {
	fs::path URI(name);

	if (name.empty() || URI.filename() != URI ) {
		throw std::invalid_argument( "Invalid name '" + name + "'");
	}

	auto manager = LockManager();

	if ( manager->d_zonesByURI.count(URI) != 0 ) {
		throw std::invalid_argument("'" +  name + "' is already used");
	}

	auto fi = manager->d_zonesByURI.find(d_URI);
	if ( fi != manager->d_zonesByURI.end() ) {
		manager->d_zonesByURI.insert(std::make_pair(URI,fi->second));
		manager->d_zonesByURI.erase(fi);
	}

	d_URI = URI;
}

const std::vector<TrackingDataDirectory::ConstPtr> & Zone::TrackingDataDirectories() const {
	return d_tdds;
}

Zone::Manager::Ptr Zone::LockManager() const {
	auto locked = d_manager.lock();
	if ( !locked ) {
		throw DeletedReference<Zone::Manager>();
	}
	return locked;
}


} //namespace priv
} //namespace myrmidon
} //namespace fort
