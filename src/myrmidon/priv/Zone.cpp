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


Zone::Zone(const std::string & name) {
	SetName(name);
}

void Zone::AddTrackingDataDirectory(const TrackingDataDirectory::ConstPtr & tdd) {
	auto newList = d_tdds;
	newList.push_back(tdd);
	auto fi = TimeValid::SortAndCheckOverlap(newList.begin(),newList.end());
	if ( fi.first != fi.second ) {
		throw TDDOverlap(*fi.first,*fi.second);
	}

	d_tdds = newList;
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
	d_URI = URI;
}

const std::vector<TrackingDataDirectory::ConstPtr> & Zone::TrackingDataDirectories() const {
	return d_tdds;
}

} //namespace priv
} //namespace myrmidon
} //namespace fort
