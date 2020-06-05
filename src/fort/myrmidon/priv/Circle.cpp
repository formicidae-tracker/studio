#include "Circle.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

Circle::Circle(const Eigen::Vector2d & center,
               double radius)
	: Shape(Shape::Type::Circle)
	, d_center(center)
	, d_radius(radius) {
}

Circle::~Circle() {
}


bool Circle::Contains(const Eigen::Vector2d & point) const {
	return (point - d_center).squaredNorm() <= d_radius*d_radius;
}

AABB Circle::ComputeAABB() const {
	Eigen::Vector2d r(d_radius,d_radius);
	return AABB( d_center - r, d_center + r);
}

} //namespace priv
} //namespace myrmidon
} //namespace fort
