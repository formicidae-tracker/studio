#include "Zone.hpp"

#include "priv/Zone.hpp"
#include <fort/myrmidon/Shapes.hpp>

#include "utils/ConstClassHelper.hpp"

namespace fort {
namespace myrmidon {

Zone::Zone(const PPtr & pZone)
	: d_p(pZone) {
}

ZoneDefinition::ZoneDefinition(const PPtr & pDefinition)
	: d_p(pDefinition) {
}

const Shape::List & ZoneDefinition::Shapes() const {
	return d_p->Shapes();
}

void ZoneDefinition::SetShapes(const Shape::List & shapes) {
	d_p->SetShapes(shapes);
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

ZoneDefinition::Ptr Zone::AddDefinition(const Shape::List & shapes,
                                        const Time & start,
                                        const Time & end) {
	return ZoneDefinition::Ptr(new ZoneDefinition(d_p->AddDefinition(shapes,
	                                                                 start,end)));
}

ZoneDefinition::List Zone::Definitions() {
	ZoneDefinition::List res;
	for ( const auto & d : d_p->Definitions() ) {
		res.push_back(ZoneDefinition::Ptr(new ZoneDefinition(d)));
	}
	return res;
}

void Zone::DeleteDefinition(size_t index) {
	d_p->EraseDefinition(index);
}

const std::string & Zone::Name() const {
	return d_p->Name();
}

void Zone::SetName(const std::string & name) {
	d_p->SetName(name);
}

ZoneID Zone::ID() const {
	return d_p->ZoneID();
}





} // namespace myrmidon
} // namespace fort
