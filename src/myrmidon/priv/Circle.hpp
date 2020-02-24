#pragma once


#include "Shape.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


class Circle : public Shape {
public:
	typedef std::shared_ptr<Circle> Ptr;
	typedef std::shared_ptr<const Circle> ConstPtr;

	Circle();
	Circle(const Eigen::Vector2d & center,
	       double radius);

	inline void SetCenter(const Eigen::Vector2d & center) {
		d_center = center;
	}

	inline const Eigen::Vector2d & Center() const {
		return d_center;
	}

	inline void SetRadius(double radius) {
		d_radius = radius;
	}

	inline double Radius() const {
		return d_radius;
	}

	bool Contains(const Eigen::Vector2d & point) const override;

	AABB ComputeAABB() const override;


private:
	Eigen::Vector2d d_center;
	double d_radius;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};


} //namespace priv
} //namespace myrmidon
} //namespace fort
