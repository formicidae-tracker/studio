#include "Shapes.hpp"

#include "priv/Shape.hpp"
#include "priv/Circle.hpp"
#include "priv/Capsule.hpp"
#include "priv/Polygon.hpp"

namespace fort {
namespace myrmidon {

Shape::~Shape() {
}

Shape::ConstPtr Shape::Cast(const ConstPPtr & pShape) {
	switch(pShape->ShapeType()) {
	case priv::Shape::Type::Capsule:
		return std::make_shared<const Capsule>(std::const_pointer_cast<priv::Capsule>(priv::Shape::ToCapsule(pShape)));
	case priv::Shape::Type::Circle:
		return std::make_shared<const Circle>(std::const_pointer_cast<priv::Circle>(priv::Shape::ToCircle(pShape)));
	case priv::Shape::Type::Polygon:
		return std::make_shared<const Polygon>(std::const_pointer_cast<priv::Polygon>(priv::Shape::ToPolygon(pShape)));
	default:
		throw std::runtime_error("Unknown shape type");

	}
}

Shape::ConstPPtr Shape::Cast(const ConstPtr & shape) {
	if ( auto c = std::dynamic_pointer_cast<const Capsule>(shape) ) {
		return std::static_pointer_cast<const priv::Shape>(c->d_p);
	}
	if ( auto c = std::dynamic_pointer_cast<const Circle>(shape) ) {
		return std::static_pointer_cast<const priv::Shape>(c->d_p);
	}
	if ( auto p = std::dynamic_pointer_cast<const Polygon>(shape) ) {
		return std::static_pointer_cast<const priv::Shape>(p->d_p);
	}
	throw std::runtime_error("Unknown shape Type");
}


Circle::Circle(const Eigen::Vector2d & center, double radius)
	: d_p(std::make_shared<priv::Circle>(center,radius)) {
}

Circle::Circle(const PPtr & pCircle)
	: d_p(pCircle) {
}

Circle::~Circle() {
}

void Circle::SetCenter( const Eigen::Vector2d & center) {
	d_p->SetCenter(center);
}

const Eigen::Vector2d & Circle::Center() const {
	return d_p->Center();
}

void Circle::SetRadius(double radius) {
	d_p->SetRadius(radius);
}

double Circle::Radius() const {
	return d_p->Radius();
}

Capsule::Capsule(const Eigen::Vector2d & c1,
                 const Eigen::Vector2d & c2,
                 double r1,
                 double r2)
	: d_p(std::make_shared<priv::Capsule>(c1,c2,r1,r2)) {
}

Capsule::Capsule(const PPtr & pCapsule)
	: d_p(pCapsule) {
}

Capsule::~Capsule() {
}

void Capsule::SetC1(const Eigen::Vector2d & c1) {
	d_p->SetC1(c1);
}

void Capsule::SetC2(const Eigen::Vector2d & c2) {
	d_p->SetC2(c2);
}

const Eigen::Vector2d & Capsule::C1() const {
	return d_p->C1();
}

const Eigen::Vector2d & Capsule::C2() const {
	return d_p->C2();
}

void Capsule::SetR1(double r1) {
	d_p->SetR1(r1);
}

void Capsule::SetR2(double r2) {
	d_p->SetR2(r2);
}

double Capsule::R1() const {
	return d_p->R1();
}
double Capsule::R2() const {
	return d_p->R2();
}

Polygon::Polygon(const Vector2dList & vertices)
	: d_p(std::make_shared<priv::Polygon>(vertices)) {
}

Polygon::Polygon(const PPtr & pPolygon)
	: d_p(pPolygon) {
}

Polygon::~Polygon() {
}

size_t Polygon::Size() const {
	return d_p->Size();
}

const Eigen::Vector2d & Polygon::Vertex(size_t i) const {
	return d_p->Vertex(i);
}

void Polygon::SetVertex(size_t i, const Eigen::Vector2d & v) {
	d_p->SetVertex(i,v);
}

} // namespace myrmidon
} // namespace fort
