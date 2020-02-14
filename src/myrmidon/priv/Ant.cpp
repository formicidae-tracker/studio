#include "Ant.hpp"

#include <sstream>
#include <iomanip>


namespace fort {
namespace myrmidon {
namespace priv {

std::string Ant::FormatID(fort::myrmidon::Ant::ID ID) {
	std::ostringstream os;
	os << "0x" << std::setw(4) << std::setfill('0') << std::hex
	   << std::uppercase << ID;
	return os.str();
}

Ant::Ant(fort::myrmidon::Ant::ID ID)
	: d_ID(ID)
	, d_IDStr(FormatID(ID))
	, d_displayColor(Palette::Default().At(0) )
	, d_displayState(DisplayState::VISIBLE) {
}

Ant::~Ant() {
}

Identification::List & Ant::Accessor::Identifications(Ant & a){
	return a.d_identifications;
}

const Identification::List & Ant::Identifications() const {
	return d_identifications;
}

const Ant::Shapes & Ant::Shape() const {
	return d_shape;
}

void Ant::AddCapsule(const Capsule::Ptr & capsule) {
	if (!capsule) {
		throw std::invalid_argument("No capsule");
	}
	d_shape.push_back(capsule);
}


void Ant::SetDisplayColor(const Color & color) {
	d_displayColor = color;
}

const Color & Ant::DisplayColor() const {
	return d_displayColor;
}

void Ant::SetDisplayStatus(Ant::DisplayState s) {
	d_displayState = s;
}

Ant::DisplayState Ant::DisplayStatus() const {
	return d_displayState;
}



} // namespace priv
} // namespace myrmidon
} // namespace fort
