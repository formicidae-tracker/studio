#pragma once

#include <Eigen/Core>

#include <memory>

namespace fort {

namespace myrmidon {

namespace priv {


class Capsule {
public:
	typedef std::shared_ptr<Capsule> Ptr;
	typedef std::shared_ptr<const Capsule> ConstPtr;

	Capsule();
	Capsule(const Eigen::Vector2d & a,
	        const Eigen::Vector2d & b,
	        double aRadius,
	        double bRadius);

	inline void SetA(const Eigen::Vector2d & a) {
		d_a = a;
	}

	inline const Eigen::Vector2d & A() const {
		return d_a;
	}

	inline void SetB(const Eigen::Vector2d & b) {
		d_b = b;
	}

	inline const Eigen::Vector2d & B() const {
		return d_b;
	}

	inline void SetRadiusA(double ra) {
		d_ra = ra;
	}

	inline double RadiusA() const {
		return d_ra;
	}

	inline void SetRadiusB(double rb) {
		d_rb = rb;
	}

	inline double RadiusB() const {
		return d_rb;
	}


private:
	Eigen::Vector2d d_a,d_b;
	double d_ra,d_rb;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};


} //namespace priv

} //namespace myrmidon

} //namespace fort
