#include "Zone.hpp"

#include "priv/Zone.hpp"
#include "priv/Shape.hpp"

namespace fort {
namespace myrmidon {

Zone::Zone(const PPtr & pZone)
	: d_p(pZone) {
}

Zone::Definition::Definition(const PPtr & pDefinition)
	: d_p(pDefinition) {
}

Shape::ConstList Zone::Definition::Geometry() const {
	return Shape::Cast(d_p->GetGeometry()->Shapes());
}

void Zone::Definition::SetGeometry(const Shape::ConstList & shapes) {
	d_p->SetGeometry(std::make_shared<priv::ZoneGeometry>(Shape::Cast(shapes)));
}

const Time::ConstPtr & Zone::Definition::Start() const {
	return d_p->Start();
}

const Time::ConstPtr & Zone::Definition::End() const {
	return d_p->End();
}

void Zone::Definition::SetStart(const Time::ConstPtr & start) {
	d_p->SetStart(start);
}


void Zone::Definition::SetEnd(const Time::ConstPtr & end) {
	d_p->SetEnd(end);
}

Zone::Definition::Ptr Zone::AddDefinition(const Shape::ConstList & geometry,
                                          const Time::ConstPtr & start,
                                          const Time::ConstPtr & end) {
	return std::make_shared<Zone::Definition>(d_p->AddDefinition(Shape::Cast(geometry),
	                                                             start,end));
}

 Zone::Definition::ConstList Zone::CDefinitions() const {
	 Definition::ConstList res;
	 for ( const auto & d : d_p->Definitions() ) {
		 res.push_back(std::make_shared<const Definition>(d));
	 }
	 return res;
 }

 Zone::Definition::List Zone::Definitions() {
	 Definition::List res;
	 for ( const auto & d : d_p->Definitions() ) {
		 res.push_back(std::make_shared<Definition>(d));
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
