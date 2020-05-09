#include "Zone.hpp"

#include "priv/Zone.hpp"
#include "priv/Shape.hpp"

namespace fort {
namespace myrmidon {

Zone::Zone(const PPtr & pZone)
	: d_p(pZone) {
}


Shape::ConstList Zone::Definition::Geometry() const {
	Shape::ConstList res;
	for ( const auto & pShape : d_p->GetGeometry()->Shapes() ) {
		res.push_back(Shape::Cast(pShape));
	}
	return res;
}

void Zone::Definition::SetGeometry(const Shape::ConstList & shapes) {
	std::vector<priv::Shape::ConstPtr> pShapes;
	for ( const auto & shape : shapes ) {
		pShapes.push_back(Shape::Cast(shape));
	}
	d_p->SetGeometry(std::make_shared<priv::ZoneGeometry>(pShapes));
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

} // namespace myrmidon
} // namespace fort
