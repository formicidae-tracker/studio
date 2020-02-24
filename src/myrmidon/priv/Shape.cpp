#include "Shape.hpp"

#include "Capsule.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

Shape::Shape(Type type)
	: d_type(type) {
}

Shape::~Shape() {
}

Shape::Type Shape::ShapeType() const {
	return d_type;
}

CapsuleConstPtr Shape::ToCapsule(const ConstPtr & s) {
	if ( s->ShapeType() != Type::Capsule ) {
		return Capsule::ConstPtr();
	}
	return std::static_pointer_cast<const Capsule>(s);
}

// CircleConstPtr  Shape::ToCircle(const ConstPtr & s) {

// }

// PolygonConstPtr Shape::ToPolygon(const ConstPtr & s) {

// }


} // namespace priv
} // namespace myrmidon
} // namespace fort
