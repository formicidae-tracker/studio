#include "Ant.hpp"

#include <sstream>
#include <iomanip>


using namespace fort::myrmidon::priv;

std::string Ant::FormatID(fort::myrmidon::Ant::ID ID) {
	std::ostringstream os;
	os << "0x" << std::setw(4) << std::setfill('0') << std::hex
	   << std::uppercase << ID;
	return os.str();
}

Ant::Ant(uint32_t ID)
	: d_ID(ID)
	, d_IDStr(FormatID(ID)) {
}

Ant::~Ant() {
}

Identification::List & Ant::Accessor::Identifications(Ant & a){
	return a.d_identifications;
}

const Identification::List & Ant::Identifications() const {
	return d_identifications;
}
