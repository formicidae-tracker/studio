#include "AntPoseEstimate.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

fs::path AntPoseEstimate::BuildURI(const FrameReference & reference,
                                   TagID tid) {
	return reference.URI() / "estimates" / std::to_string(tid);
}


AntPoseEstimate::AntPoseEstimate(const FrameReference & reference,
                                 TagID tid,
                                 const Eigen::Vector2d & position,
                                 double angle)
	: d_x(position.x())
	, d_y(position.y())
	, d_angle(angle)
	, d_reference(reference)
	, d_tid(tid)
	, d_URI(BuildURI(reference,tid)) {
}


AntPoseEstimate::AntPoseEstimate(const FrameReference & reference,
                                 TagID tid,
                                 const Eigen::Vector2d & headFromTag,
                                 const Eigen::Vector2d & tailFromTag)
	: d_x(0.0)
	, d_y(0.0)
	, d_angle(0.0)
	, d_reference(reference)
	, d_tid(tid)
	, d_URI(BuildURI(reference,tid)) {

	Eigen::Vector2d diff = (headFromTag - tailFromTag);
	Eigen::Vector2d center = (headFromTag + tailFromTag) / 2.0;
	if ( diff.norm() < 1.0 ) {
		throw std::runtime_error("Too small difference between head and tail position");
	}

	d_angle = std::atan2(diff.y(),diff.x());
	d_x = center.x();
	d_y = center.y();
}

const fs::path & AntPoseEstimate::URI() const {
	return d_URI;
}

const FrameReference & AntPoseEstimate::Reference() const {
	return d_reference;
}

Eigen::Vector2d AntPoseEstimate::PositionFromTag() const {
	return Eigen::Vector2d(d_x,d_y);
}

double AntPoseEstimate::AngleFromTag() const {
	return d_angle;
}

TagID  AntPoseEstimate::TargetTagID() const {
	return d_tid;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
