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


bool Circle::Contains(const Eigen::Vector2d & point) const {
	return (point - d_center).squaredNorm() <= d_radius*d_radius;
}

} //namespace priv
} //namespace myrmidon
} //namespace fort
