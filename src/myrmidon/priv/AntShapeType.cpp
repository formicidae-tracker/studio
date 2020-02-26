#include "AntShapeType.hpp"



namespace fort {
namespace myrmidon {
namespace priv {

AntShapeType::AntShapeType(ID TypeID, const std::string & name)
	: d_ID(TypeID)
	, d_name(name) {
}

const std::string & AntShapeType::Name() const {
	return d_name;
}

void AntShapeType::SetName(const std::string & name) {
	d_name = name;
}

AntShapeType::ID AntShapeType::TypeID() const {
	return d_ID;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
