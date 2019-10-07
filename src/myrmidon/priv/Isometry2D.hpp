#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
namespace fort {

namespace myrmidon {

namespace priv {

template <typename T>
class Isometry2D;

} // namespace priv

} // namespace myrmidon

} // namespace fort

template<typename T>
fort::myrmidon::priv::Isometry2D<T> operator*(const fort::myrmidon::priv::Isometry2D<T> & a,
                                              const fort::myrmidon::priv::Isometry2D<T> & b);

template<typename T>
Eigen::Matrix<T,2,1> operator*(const fort::myrmidon::priv::Isometry2D<T> & i,
                               const Eigen::Matrix<T,2,1> & p);


namespace fort {

namespace myrmidon {

namespace priv {

template<typename T>
class Isometry2D {
public:
	Isometry2D() {}
	Isometry2D(T angle, const Eigen::Matrix<T,2,1> & translation)
		: d_angle(angle)
		, d_translation(translation) { }

	const Eigen::Rotation2D<T> & rotation() const {
		return Eigen::Rotation2D<T>(d_angle);
	}

	T angle() const {
		return d_angle;
	}

	const Eigen::Matrix<T,2,1> & translation() const {
		return d_translation;
	}

	Isometry2D<T> inverse() const {
		return Isometry2D<T>(-d_angle,Eigen::Rotation2D<T>(-d_angle)*(-d_translation));
	}


private:
	double d_angle;
	Eigen::Matrix<T,2,1> d_translation;
	 enum { NeedsToAlign = (sizeof(Eigen::Matrix<T,2,1>)%16)==0 };
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(NeedsToAlign);


	friend fort::myrmidon::priv::Isometry2D<T> operator*<>(const fort::myrmidon::priv::Isometry2D<T> & a,
	                                                       const fort::myrmidon::priv::Isometry2D<T> & b);

	friend Eigen::Matrix<T,2,1> operator*<>(const fort::myrmidon::priv::Isometry2D<T> & i,
	                                        const Eigen::Matrix<T,2,1> & p);

};

typedef Isometry2D<double> Isometry2Dd;



} // namespace priv

} // namespace myrmidon

} // namespace fort

template<typename T>
inline Eigen::Matrix<T,2,1> operator*(const fort::myrmidon::priv::Isometry2D<T> & i,
                                      const Eigen::Matrix<T,2,1> & p) {
	return Eigen::Rotation2D<T>(i.d_angle) * p + i.d_translation;
}

template<typename T>
inline fort::myrmidon::priv::Isometry2D<T> operator*(const fort::myrmidon::priv::Isometry2D<T> & a,
                                                     const fort::myrmidon::priv::Isometry2D<T> & b) {
	return fort::myrmidon::priv::Isometry2D<T>(a.d_angle + b.d_angle,
	                                           Eigen::Rotation2D<T>(a.d_angle) * b.d_translation + a.d_translation);
}
