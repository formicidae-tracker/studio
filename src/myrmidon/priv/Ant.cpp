#include "Ant.hpp"

#include <sstream>
#include <iomanip>


using namespace fort::myrmidon::priv;

Ant::Ant(fort::myrmidon::pb::AntMetadata * ant)
	: d_metadata(ant) {
	std::ostringstream os;
	os << "0x" << std::setw(4) << std::setfill('0') << std::hex
	   << std::uppercase << d_metadata->id();
	d_ID = os.str();
}

Ant::~Ant() {
}


Ant::Identification::List & Ant::Identifications(){
	return d_identifications;
}

void Ant::SortAndCheckIdentifications() {
	std::sort(d_identifications.begin(),
	          d_identifications.end(),
	          [](const Identification & a,
	             const Identification & b) -> bool {
		          if ( !a.Start ) {
			          return true;
		          }
		          if ( !b.Start ) {
			          return false;
		          }
		          return *a.Start < *b.Start;
	          });
	if (d_identifications.size() < 2 ) {
		return;
	}

	for ( auto i = d_identifications.cbegin() + 1;
	      i != d_identifications.cend();
	      ++i) {
		auto prev = i-1;
		if ( !(i->Start) || !(prev->End) || !(*(prev->End) < *(i->Start)) ) {
			throw OverlappingIdentification(*prev,*i);
		}
	}

}


std::ostream & operator<<(std::ostream & out, const Ant::Identification & a) {
	out << "Identification{ID:"
	    << a.TagValue
	    << ", From:'";
	if (a.Start) {
		out << a.Start->Path << "/" << a.Start->Frame;
	} else {
		out << "<begin>";
	}
	out << "', To:'";
	if (a.End) {
		out << a.End->Path << "/" << a.End->Frame;
	} else {
		out << "<end>";
	}
	return out << "'}";

}


Ant::OverlappingIdentification::OverlappingIdentification(const Identification & a,
                                                          const Identification & b)
	: std::runtime_error(Reason(a,b)){
}
std::string Ant::OverlappingIdentification::Reason(const Identification & a,
                                                   const Identification & b) {
	std::ostringstream os;
	os << a << " and " << b << " overlaps";
	return os.str();
}
