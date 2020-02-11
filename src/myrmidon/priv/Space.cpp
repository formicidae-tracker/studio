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

Space::UnmanagedTrackingDataDirectory::UnmanagedTrackingDataDirectory(const fs::path & URI) noexcept
	: std::runtime_error("TDD:'" + URI.generic_string() + "' is not managed by this Space or Universe") {
}

Space::UnmanagedSpace::UnmanagedSpace(const fs::path & URI) noexcept
	: std::runtime_error("Space:'" + URI.generic_string() + "' is not managed by this Universe") {
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
	oss << "Space:'" << z.URI().generic_string()
	    << "' is not empty (contains:";
	std::string sep = "{";
	for ( const auto & tdd : z.d_tdds ) {
		oss << sep << tdd->URI().generic_string();
		sep = ",";
	}
	oss << "})";
	return oss.str();
}

Space::TDDAlreadyInUse::TDDAlreadyInUse(const fs::path & tddURI, const fs::path & spaceURI)
	: std::runtime_error("TDD:'"
	                     + tddURI.generic_string()
	                     + "' is in use in Space:'"
	                     + spaceURI.generic_string() + "'") {
}

Space::Ptr Space::Universe::Create(const Ptr & itself, const std::string & name) {
	std::shared_ptr<Space> res(new Space(name,itself));
	itself->d_spaces.push_back(res);
	return res;
}

void Space::Universe::DeleteSpace(const fs::path & URI) {
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

void Space::Universe::DeleteTrackingDataDirectory(const fs::path & URI) {
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
	: d_universe(universe) {
	SetName(name);
}

const Space::Universe::TrackingDataDirectoryByURI &
Space::Universe::TrackingDataDirectories() const {
	return d_tddsByURI;
}


void Space::AddTrackingDataDirectory(const TrackingDataDirectory::ConstPtr & tdd) {
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
		                         + tdd->URI().generic_string()
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

void Space::DeleteTrackingDataDirectory(const fs::path & URI) {
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

const fs::path & Space::URI() const {
	return d_URI;
}

void Space::SetName(const std::string & name) {
	fs::path URI(name);

	if (name.empty()) {
		throw InvalidName(name,"it is empty");
	}
	if (URI.filename() != URI) {
		throw InvalidName(name,"invalid character in path");
	}

	auto universe = LockUniverse();


	auto zi = std::find_if(universe->d_spaces.begin(),
	                       universe->d_spaces.end(),
	                       [URI](const Space::Ptr & z ) {
		                       return z->URI() == URI;
	                       });
	if (zi != universe->d_spaces.end()) {
		throw InvalidName(name,"is already used by another Space");
	}

	if ( universe->d_tddsByURI.count(URI) != 0 ) {
		throw InvalidName(name,"is already used by another TDD");
	}

	d_URI = URI;
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
Space::Universe::LocateTrackingDataDirectory(const fs::path & tddURI) const {
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

Space::Ptr Space::Universe::LocateSpace(const fs::path & spaceURI) const {
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



} //namespace priv
} //namespace myrmidon
} //namespace fort
