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

	oss << *a << " and " << *b << " overlaps in time";

	return oss.str();
}

Zone::UnmanagedTrackingDataDirectory::UnmanagedTrackingDataDirectory(const fs::path & URI) noexcept
	: std::runtime_error("TDD:'" + URI.generic_string() + "' is not managed by this Zone or Zone::Group") {
}

Zone::UnmanagedZone::UnmanagedZone(const fs::path & URI) noexcept
	: std::runtime_error("Zone:'" + URI.generic_string() + "' is not managed by this Zone::Group") {
}

Zone::InvalidName::InvalidName(const std::string & name,
                             const std::string & reason) noexcept
	: std::runtime_error("Invalid Zone name '" + name + "': " + reason) {
}

Zone::ZoneNotEmpty::ZoneNotEmpty(const Zone & z)
	: std::runtime_error(BuildReason(z)) {
}

std::string Zone::ZoneNotEmpty::BuildReason(const Zone & z) {
	std::ostringstream oss;
	oss << "Zone:'" << z.URI().generic_string()
	    << "' is not empty (contains:";
	std::string sep = "{";
	for ( const auto & tdd : z.d_tdds ) {
		oss << sep << tdd->URI().generic_string();
		sep = ",";
	}
	oss << "})";
	return oss.str();
}

Zone::TDDAlreadyInUse::TDDAlreadyInUse(const fs::path & tddURI, const fs::path & zoneURI)
	: std::runtime_error("TDD:'"
	                     + tddURI.generic_string()
	                     + "' is in use in Zone:'"
	                     + zoneURI.generic_string() + "'") {
}

Zone::Ptr Zone::Group::Create(const Ptr & itself, const std::string & name) {
	std::shared_ptr<Zone> res(new Zone(name,itself));
	itself->d_zones.push_back(res);
	return res;
}

void Zone::Group::DeleteZone(const fs::path & URI) {
	auto fi = std::find_if(d_zones.begin(),
	                       d_zones.end(),
	                       [URI](const Zone::Ptr & z) -> bool {
		                       return z->URI() == URI;
	                       });
	if ( fi == d_zones.end() ) {
		throw UnmanagedZone(URI);
	}

	if ( (*fi)->d_tdds.empty() == false ) {
		throw ZoneNotEmpty(**fi);
	}

	d_zones.erase(fi);
}

void Zone::Group::DeleteTrackingDataDirectory(const fs::path & URI) {
	for ( const auto & z : d_zones ) {
		try {
			z->DeleteTrackingDataDirectory(URI);
			return;
		} catch ( const UnmanagedTrackingDataDirectory & e) {
		}
	}
	throw UnmanagedTrackingDataDirectory(URI);
}

const std::vector<Zone::Ptr> & Zone::Group::Zones() const {
	return d_zones;
}

Zone::Zone(const std::string & name, const Group::Ptr & group)
	: d_group(group) {
	SetName(name);
}

const Zone::Group::TrackingDataDirectoryByURI &
Zone::Group::TrackingDataDirectories() const {
	return d_tddsByURI;
}


void Zone::AddTrackingDataDirectory(const TrackingDataDirectory::ConstPtr & tdd) {
	auto newList = d_tdds;
	newList.push_back(tdd);
	auto fi = TimeValid::SortAndCheckOverlap(newList.begin(),newList.end());
	if ( fi.first != fi.second ) {
		throw TDDOverlap(*fi.first,*fi.second);
	}

	auto group = LockGroup();

	auto zi = std::find_if(group->d_zones.begin(),
	                       group->d_zones.end(),
	                       [&tdd](const Zone::Ptr & z) {
		                       return z->URI() == tdd->URI();
	                       });

	if ( zi != group->d_zones.end() ) {
		throw std::runtime_error("TDD:'"
		                         + tdd->URI().generic_string()
		                         + " has the same than a Zone in this group");
	}

	if ( group->d_tddsByURI.count(tdd->URI()) != 0 ) {
		auto zi = std::find_if(group->d_zones.begin(),
		                       group->d_zones.end(),
		                       [&tdd](const  Zone::Ptr  &z) {
			                       auto ti = std::find_if(z->d_tdds.begin(),
			                                              z->d_tdds.end(),
			                                              [&tdd](const TrackingDataDirectory::ConstPtr & tdd) {
				                                              return tdd->URI() == tdd->URI();
			                                              });
			                       return ti != z->d_tdds.end();
		                       });
		if ( zi == group->d_zones.end()){
			throw std::logic_error("Internal data error");
		}

		throw TDDAlreadyInUse(tdd->URI(),(*zi)->URI());
	}

	d_tdds = newList;
	group->d_tddsByURI.insert(std::make_pair(tdd->URI(),tdd));
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
	auto group = LockGroup();
	group->d_tddsByURI.erase(URI);

	d_tdds.erase(fi);
}

const fs::path & Zone::URI() const {
	return d_URI;
}

void Zone::SetName(const std::string & name) {
	fs::path URI(name);

	if (name.empty()) {
		throw InvalidName(name,"it is empty");
	}
	if (URI.filename() != URI) {
		throw InvalidName(name,"invalid character in path");
	}

	auto group = LockGroup();


	auto zi = std::find_if(group->d_zones.begin(),
	                       group->d_zones.end(),
	                       [URI](const Zone::Ptr & z ) {
		                       return z->URI() == URI;
	                       });
	if (zi != group->d_zones.end()) {
		throw InvalidName(name,"is already used by another Zone");
	}

	if ( group->d_tddsByURI.count(URI) != 0 ) {
		throw InvalidName(name,"is already used by another TDD");
	}

	d_URI = URI;
}

const std::vector<TrackingDataDirectory::ConstPtr> & Zone::TrackingDataDirectories() const {
	return d_tdds;
}

Zone::Group::Ptr Zone::LockGroup() const {
	auto locked = d_group.lock();
	if ( !locked ) {
		throw DeletedReference<Zone::Group>();
	}
	return locked;
}

std::pair<Zone::Ptr,TrackingDataDirectory::ConstPtr>
Zone::Group::LocateTrackingDataDirectory(const fs::path & tddURI) const {
	auto tddi = d_tddsByURI.find(tddURI) ;
	if ( tddi == d_tddsByURI.end() ) {
		return std::make_pair(Zone::Ptr(),TrackingDataDirectory::ConstPtr());
	}

	auto zi = std::find_if(d_zones.begin(),
	                       d_zones.end(),
	                       [&tddURI]( const Zone::Ptr & z) {
		                       auto ti = std::find_if(z->d_tdds.begin(),
		                                              z->d_tdds.end(),
		                                              [&tddURI]( const TrackingDataDirectory::ConstPtr & tdd) {
			                                              return tdd->URI() == tddURI;
		                                              });
		                       return ti != z->d_tdds.end();
	                       });
	if ( zi ==  d_zones.end()) {
		return std::make_pair(Zone::Ptr(),TrackingDataDirectory::ConstPtr());
	}

	return std::make_pair(*zi,tddi->second);
}

Zone::Ptr Zone::Group::LocateZone(const fs::path & zoneURI) const {
	auto zi = std::find_if(d_zones.begin(),
	                       d_zones.end(),
	                       [&zoneURI] ( const Zone::Ptr & z) {
		                       return z->URI() == zoneURI;
	                       });
	if ( zi == d_zones.end()) {
		return Zone::Ptr();
	}
	return *zi;
}



} //namespace priv
} //namespace myrmidon
} //namespace fort
