#pragma once

#include <fort/myrmidon/Vector2d.pb.h>
#include <fort/myrmidon/Time.hpp>

#include <Eigen/Core>

#include <QPointF>

namespace fort {
namespace myrmidon {
namespace pb {

inline Eigen::Vector2d Point2dToEigen(const fort::myrmidon::pb::Vector2d & p) {
	return Eigen::Vector2d(p.x(),p.y());
}

inline void EigenToPoint2d(fort::myrmidon::pb::Vector2d * pb, const Eigen::Vector2d & v) {
	pb->set_x(v.x());
	pb->set_y(v.y());
}


}
}
}

inline Eigen::Vector2d ToEigen(const QPointF & a) {
	return Eigen::Vector2d(a.x(),a.y());
}

inline bool TimePtrEquals(const fort::myrmidon::Time::ConstPtr & a,
                          const fort::myrmidon::Time::ConstPtr & b) {
	if ( !a ) {
		return !b;
	}
	if (!b) {
		return false;
	}
	return a->Equals(*b);
}
