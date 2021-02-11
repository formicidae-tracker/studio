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
	return FORT_MYRMIDON_CONST_HELPER(ZoneDefinition,Geometry);
}

Shape::ConstList CZoneDefinition::Geometry() const {
	return Shape::Cast(d_p->GetGeometry()->Shapes());
}

void ZoneDefinition::SetGeometry(const Shape::ConstList & shapes) {
	d_p->SetGeometry(std::make_shared<priv::ZoneGeometry>(Shape::Cast(shapes)));
}

const Time & ZoneDefinition::Start() const {
	return FORT_MYRMIDON_CONST_HELPER(ZoneDefinition,Start);
}

const Time & CZoneDefinition::Start() const {
	return d_p->Start();
}

const Time & ZoneDefinition::End() const {
	return FORT_MYRMIDON_CONST_HELPER(ZoneDefinition,End);
}

const Time & CZoneDefinition::End() const {
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
	return ZoneDefinition(d_p->AddDefinition(Shape::Cast(geometry),
	                                         start,end));
}

ZoneDefinition::ConstList Zone::CDefinitions() const {
	return FORT_MYRMIDON_CONST_HELPER(Zone,CDefinitions);
}

ZoneDefinition::ConstList CZone::CDefinitions() const {
	ZoneDefinition::ConstList res;
	for ( const auto & d : d_p->CDefinitions() ) {
		res.push_back(CZoneDefinition(d));
	}
	return res;
 }

ZoneDefinition::List Zone::Definitions() {
	ZoneDefinition::List res;
	for ( const auto & d : d_p->Definitions() ) {
		res.push_back(ZoneDefinition(d));
	}
	return res;
}

void Zone::EraseDefinition(size_t index) {
	d_p->EraseDefinition(index);
}

const std::string & Zone::Name() const {
	return FORT_MYRMIDON_CONST_HELPER(Zone,Name);
}

const std::string & CZone::Name() const {
	return d_p->Name();
}

void Zone::SetName(const std::string & name) {
	d_p->SetName(name);
}

fort::myrmidon::ZoneID Zone::ZoneID() const {
	return FORT_MYRMIDON_CONST_HELPER(Zone,ZoneID);
}

Zone::ID CZone::ZoneID() const {
	return d_p->ZoneID();
}

CZoneDefinition::CZoneDefinition(const ConstPPtr & pDefinition)
	: d_p(pDefinition) {
}

CZone::CZone(const ConstPPtr & pZone)
	: d_p(pZone) {
}



} // namespace myrmidon
} // namespace fort
