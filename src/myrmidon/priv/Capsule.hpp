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
	Capsule(const Eigen::Vector2d & c1,
	        const Eigen::Vector2d & c2,
	        double r1,
	        double r2);

	inline void SetC1(const Eigen::Vector2d & c1) {
		d_c1 = c1;
	}

	inline const Eigen::Vector2d & C1() const {
		return d_c1;
	}

	inline void SetC2(const Eigen::Vector2d & c2) {
		d_c2 = c2;
	}

	inline const Eigen::Vector2d & C2() const {
		return d_c2;
	}

	inline void SetR1(double r1) {
		d_r1 = r1;
	}

	inline double R1() const {
		return d_r1;
	}

	inline void SetR2(double r2) {
		d_r2 = r2;
	}

	inline double R2() const {
		return d_r2;
	}


	static bool Intersect(const Eigen::Vector2d & aC1,
	                      const Eigen::Vector2d & aC2,
	                      double aR1,
	                      double aR2,
	                      const Eigen::Vector2d & bC1,
	                      const Eigen::Vector2d & bC2,
	                      double bR1,
	                      double bR2);

private:
	Eigen::Vector2d d_c1,d_c2;
	double d_r1,d_r2;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};


} //namespace priv
} //namespace myrmidon
} //namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::Capsule & c);
