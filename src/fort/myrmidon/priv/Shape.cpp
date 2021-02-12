#include "Shape.hpp"

#include "Capsule.hpp"
#include "Circle.hpp"
#include "Polygon.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

Shape::Shape(myrmidon::Shape::Type type)
	: d_type(type) {
}

Shape::~Shape() {
}

myrmidon::Shape::Type Shape::ShapeType() const {
	return d_type;
}


#define implement_caster(SType) \
SType ## ConstPtr Shape::To ## SType(const ConstPtr & s) { \
	if ( s->ShapeType() != myrmidon::Shape::Type::SType ) { \
		return SType::ConstPtr(); \
	} \
	return std::static_pointer_cast<const SType>(s); \
} \

implement_caster(Capsule);
implement_caster(Circle);
implement_caster(Polygon);

} // namespace priv
} // namespace myrmidon
} // namespace fort
