#pragma once

#include <myrmidon/Vector2d.pb.h>

#include <Eigen/Core>

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
