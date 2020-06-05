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



AntShapeType::Ptr AntShapeTypeContainer::Create(const std::string & name,
                                                AntShapeType::ID typeID) {
	return d_container.CreateObject([&name](AntShapeTypeID ID) {
		                                return std::make_shared<AntShapeType>(ID,name);
	                                },typeID);
}

void AntShapeTypeContainer::Delete(AntShapeType::ID typeID) {
	d_container.DeleteObject(typeID);
}

AntShapeType::ConstByID::const_iterator
AntShapeTypeContainer::Find(AntShapeType::ID typeID) const {
	return d_container.CObjects().find(typeID);
}

AntShapeType::ConstByID::const_iterator
AntShapeTypeContainer::End() const {
	return d_container.CObjects().cend();
}

size_t AntShapeTypeContainer::Count(AntShapeType::ID typeID) const {
	return d_container.Count(typeID);
}

const AntShapeType::ByID & AntShapeTypeContainer::Types() {
	return d_container.Objects();
}

const AntShapeType::ConstByID & AntShapeTypeContainer::CTypes() const {
	return d_container.CObjects();
}



} // namespace priv
} // namespace myrmidon
} // namespace fort
