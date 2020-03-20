#include "Ant.hpp"

#include <sstream>
#include <iomanip>

#include "AntShapeType.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

std::string Ant::FormatID(fort::myrmidon::Ant::ID ID) {
	std::ostringstream os;
	os << "0x" << std::setw(4) << std::setfill('0') << std::hex
	   << std::uppercase << ID;
	return os.str();
}

Ant::Ant(const AntShapeTypeContainerConstPtr & shapeTypeContainer,
         const AntMetadataConstPtr & metadata,
         fort::myrmidon::Ant::ID ID)
	: d_ID(ID)
	, d_IDStr(FormatID(ID))
	, d_displayColor(Palette::Default().At(0) )
	, d_displayState(DisplayState::VISIBLE)
	, d_shapeTypes(shapeTypeContainer)
	, d_metadata(metadata) {
}

Ant::~Ant() {
}

Identification::List & Ant::Accessor::Identifications(Ant & a){
	return a.d_identifications;
}

const Identification::List & Ant::Identifications() const {
	return d_identifications;
}

const Ant::TypedCapsuleList & Ant::Capsules() const {
	return d_capsules;
}

void Ant::DeleteCapsule(size_t index) {
	if ( index >= d_capsules.size() ) {
		throw std::out_of_range("Index "
		                        + std::to_string(index)
		                        + " is out of range [0;"
		                        + std::to_string(d_capsules.size())
		                        + "[");
	}
	d_capsules.erase(d_capsules.begin() + index);
}

void Ant::ClearCapsules() {
	d_capsules.clear();
}

void Ant::AddCapsule(AntShapeTypeID typeID, const CapsulePtr & capsule) {
	if ( d_shapeTypes->Count(typeID) == 0 ) {
		throw std::invalid_argument("Unknown AntShapeTypeID " + std::to_string(typeID));
	}
	if (!capsule) {
		throw std::invalid_argument("No capsule");
	}
	d_capsules.push_back(std::make_pair(typeID,capsule));
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
