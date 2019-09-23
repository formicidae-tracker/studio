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