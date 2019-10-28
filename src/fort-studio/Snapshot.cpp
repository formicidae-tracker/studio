#include "Snapshot.hpp"
#include <myrmidon/priv/FramePointer.hpp>


Eigen::Vector2d Snapshot::TagPosition() const {
	return d_position.block<2,1>(0,0);
}

double Snapshot::TagAngle() const {
	return d_position.z();
}

const Snapshot::Vector2dList & Snapshot::Corners() const {
	return d_corners;
}

fs::path Snapshot::ImagePath() const {
	return Base() / d_relativeImagePath;
}

double ComputeAngleFromCorner(const apriltag_detection_t *q) {

	Eigen::Vector2d c0(q->p[0][0],q->p[0][1]);
	Eigen::Vector2d c1(q->p[1][0],q->p[1][1]);
	Eigen::Vector2d c2(q->p[2][0],q->p[2][1]);
	Eigen::Vector2d c3(q->p[3][0],q->p[3][1]);

	Eigen::Vector2d delta = (c1 + c2) / 2.0 - (c0 + c3) / 2.0;


	return atan2(delta.y(),delta.x());
}

Snapshot::ConstPtr Snapshot::FromApriltag(const apriltag_detection_t * d,
                                          const fs::path & relativeImagePath,
                                          const fort::myrmidon::priv::FramePointer::Ptr & frame) {
	auto res = std::make_shared<Snapshot>(frame,d->id);
	res->d_relativeImagePath = relativeImagePath;

	res->d_position <<
		d->c[0],
		d->c[1],
		ComputeAngleFromCorner(d);
	for(size_t i = 0; i < 4; ++i) {
		res->d_corners.push_back(Eigen::Vector2d(d->p[i][0],d->p[i][1]));
	}
	return res;
}


Snapshot::Snapshot(const fort::myrmidon::priv::FramePointer::Ptr& frame,
                   uint32_t tagValue)
	: TagInFramePointer(frame,tagValue) {

}

Snapshot::~Snapshot() {}
