#include "Snapshot.hpp"
#include <myrmidon/priv/FramePointer.hpp>

uint32_t Snapshot::TagValue() const{
	return d_value;
}

Eigen::Vector2d Snapshot::TagPosition() const {
	return d_position.block<2,1>(0,0);
}

double Snapshot::TagAngle() const {
	return d_position.z();
}

const Snapshot::Vector2dList & Snapshot::Corners() const {
	return d_corners;
}

std::filesystem::path Snapshot::ImagePath() const {
	return d_frame->Path / d_relativeImagePath;
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
                                          const std::filesystem::path & relativeImagePath,
                                          const fort::myrmidon::priv::FramePointerPtr & frame) {
	auto res = std::make_shared<Snapshot>();
	res->d_frame = frame;
	res->d_relativeImagePath = relativeImagePath;
	res->d_value = d->id;

	res->d_position <<
		d->c[0],
		d->c[1],
		ComputeAngleFromCorner(d);
	for(size_t i = 0; i < 4; ++i) {
		res->d_corners.push_back(Eigen::Vector2d(d->p[i][0],d->p[i][1]));
	}
	return res;
}

std::filesystem::path Snapshot::Path() const {
	std::ostringstream os;
	os << d_value;
	return d_frame->FullPath() / os.str();
}

fort::myrmidon::priv::FramePointer::Ptr Snapshot::Frame() const {
	return d_frame;
}
