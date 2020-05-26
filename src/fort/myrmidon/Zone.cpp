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
	return Shape::Cast(d_p->GetGeometry()->Shapes());
}

void ZoneDefinition::SetGeometry(const Shape::ConstList & shapes) {
	d_p->SetGeometry(std::make_shared<priv::ZoneGeometry>(Shape::Cast(shapes)));
}

const Time::ConstPtr & ZoneDefinition::Start() const {
	return d_p->Start();
}

const Time::ConstPtr & ZoneDefinition::End() const {
	return d_p->End();
}

void ZoneDefinition::SetStart(const Time::ConstPtr & start) {
	d_p->SetStart(start);
}


void ZoneDefinition::SetEnd(const Time::ConstPtr & end) {
	d_p->SetEnd(end);
}

ZoneDefinition Zone::AddDefinition(const Shape::ConstList & geometry,
                                   const Time::ConstPtr & start,
                                   const Time::ConstPtr & end) {
	return ZoneDefinition(d_p->AddDefinition(Shape::Cast(geometry),
	                                         start,end));
}

ZoneDefinition::ConstList Zone::CDefinitions() const {
	ZoneDefinition::ConstList res;
	for ( const auto & d : d_p->Definitions() ) {
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
	return d_p->Name();
}

void Zone::SetName(const std::string & name) {
	d_p->SetName(name);
}

Zone::ID Zone::ZoneID() const {
	return d_p->ZoneID();
}

Shape::ConstList CZoneDefinition::Geometry() const {
	return FORT_MYRMIDON_CONST_HELPER(ZoneDefinition,Geometry);
}

const Time::ConstPtr & CZoneDefinition::Start() const {
	return FORT_MYRMIDON_CONST_HELPER(ZoneDefinition,Start);
}

const Time::ConstPtr & CZoneDefinition::End() const {
	return FORT_MYRMIDON_CONST_HELPER(ZoneDefinition,End);
}

CZoneDefinition::CZoneDefinition(const ConstPPtr & pDefinition)
	: d_p(pDefinition) {
}


ZoneDefinition::ConstList CZone::CDefinitions() const {
	return FORT_MYRMIDON_CONST_HELPER(Zone,CDefinitions);
}

const std::string & CZone::Name() const {
	return FORT_MYRMIDON_CONST_HELPER(Zone,Name);
}

fort::myrmidon::ZoneID CZone::ZoneID() const {
	return FORT_MYRMIDON_CONST_HELPER(Zone,ZoneID);
}

CZone::CZone(const ConstPPtr & pZone)
	: d_p(pZone) {
}



} // namespace myrmidon
} // namespace fort
