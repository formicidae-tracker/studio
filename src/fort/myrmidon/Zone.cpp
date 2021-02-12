#include "Zone.hpp"

#include "priv/Zone.hpp"
#include "priv/Shape.hpp"

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {

Zone::Zone(const PPtr & pZone)
	: d_p(pZone) {
}

ZoneDefinition::ZoneDefinition(const PPtr & pDefinition)
	: d_p(pDefinition) {
}

Shape::ConstList ZoneDefinition::Geometry() const {
	return Shape::PublicListCast(d_p->GetGeometry()->Shapes());
}

void ZoneDefinition::SetGeometry(const Shape::ConstList & shapes) {
	d_p->SetGeometry(std::make_shared<priv::ZoneGeometry>(Shape::PrivateListCast(shapes)));
}


const Time & ZoneDefinition::Start() const {
	return d_p->Start();
}

const Time & ZoneDefinition::End() const {
	return d_p->End();
}

void ZoneDefinition::SetStart(const Time & start) {
	d_p->SetStart(start);
}

void ZoneDefinition::SetEnd(const Time & end) {
	d_p->SetEnd(end);
}

ZoneDefinition Zone::AddDefinition(const Shape::ConstList & geometry,
                                   const Time & start,
                                   const Time & end) {
	return ZoneDefinition(d_p->AddDefinition(Shape::PrivateListCast(geometry),
	                                         start,end));
}

ZoneDefinition::ConstList Zone::Definitions() const {
	return CDefinitions();
}

ZoneDefinition::ConstList Zone::CDefinitions() const {
	ZoneDefinition::ConstList res;
	for ( const auto & d : d_p->CDefinitions() ) {
		res.push_back(std::make_shared<ZoneDefinition>(std::const_pointer_cast<priv::ZoneDefinition>(d)));
	}
	return res;
 }

ZoneDefinition::List Zone::Definitions() {
	ZoneDefinition::List res;
	for ( const auto & d : d_p->Definitions() ) {
		res.push_back(std::make_shared<ZoneDefinition>(d));
	}
	return res;
}

void Zone::EraseDefinition(size_t index) {
	d_p->EraseDefinition(index);
}

const std::string & Zone::Name() const {
	return d_p->Name();
}

void Zone::SetName(const std::string & name) {
	d_p->SetName(name);
}

Zone::ID Zone::ZoneID() const {
	return d_p->ZoneID();
}





} // namespace myrmidon
} // namespace fort
