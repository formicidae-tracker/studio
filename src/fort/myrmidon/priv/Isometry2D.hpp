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

// Applies the <fort::myrmidon::priv::Isometry2D> to a point.
// @ T the scalar type such as float or double
// @i the <fort::myrmidon::priv::Isometry2D> to apply
// @p the 2D point to apply the transformation
//
// Note: this apply a point transformation. The vector represent a
// point in a coordinate system, and not a direction. To transform a
// vector please use i.rotation()*p
//
// @return a new point representing the tranformed point.
template<typename T>
fort::myrmidon::priv::Isometry2D<T> operator*(const fort::myrmidon::priv::Isometry2D<T> & a,
                                              const fort::myrmidon::priv::Isometry2D<T> & b);

// Concatenates two <fort::myrmidon::priv::Isometry2D>
// @T the scalar type to use, float or double
// @a the second transformation to apply
// @b the first transformation to apply
//
// Note that `a * b` could be different than `b * a` most of the time.
//
// @return the two concatened isometry <b>, then <a>, as an <fort::myrmidon::priv::Isometry2D>
template<typename T>
Eigen::Matrix<T,2,1> operator*(const fort::myrmidon::priv::Isometry2D<T> & i,
                               const Eigen::Matrix<T,2,1> & p);


namespace fort {

namespace myrmidon {

namespace priv {

// Represents a 2D isometric transformation
// @T the scalar type used to represent the transformation, either double or float
//
// Represents 2D isometry using a homogenous matrices representation,
// defined by an angle and a translation.
//
// <inverse> Could be used to get the inverse() transformation and the
// <operator*> could be used to concatenate <isometry2d> or to apply
// the transformation to a 2D point.
//
//
// An Isometry2D could be used to defines the transformation from a 2D
// reference to another. whihc is useful to compute <Ant> to <Ant>
// Interaction or to get the <Ant> reference system from a tag
// reference system through the <Identification::AntToTagTransform>.
template<typename T>
class Isometry2D {
public:
	// Undefined default constructor.
	//
	// The isometry is undefined.
	Isometry2D() {}

	// Defines an Isometry2d
	// @angle: the rotation angle
	// @translation: the translation

	Isometry2D(T angle, const Eigen::Matrix<T,2,1> & translation)
		: d_angle(angle)
		, d_translation(translation) {
		while(d_angle < -M_PI ) {
			d_angle += 2.0 * M_PI;
		}
		while( d_angle >= M_PI ) {
			d_angle -= 2.0 * M_PI;
		}
	}

	// gets the rotation part if the transformation
	//
	// @return the rotation part of the isometry
	Eigen::Rotation2D<T> rotation() const {
		return Eigen::Rotation2D<T>(d_angle);
	}

	// gets the rotation angle in radian
	//
	// @return the rotation angle in radian
	T angle() const {
		return d_angle;
	}

	// gets the translation part of the isometry
	//
	// @return the translation part of the isometry
	const Eigen::Matrix<T,2,1> & translation() const {
		return d_translation;
	}

	// inverses the isometry
	//
	// @return a new <Isometry2D> representing the inverse transformation
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

// An alias for <Isometry2D<double>>
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
