#include "TagCloseUp.hpp"

#include <myrmidon/utils/Checker.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


TagCloseUp::TagCloseUp(const fs::path & absoluteFilePath,
                       const FrameReference & reference,
                       TagID tid,
                       const Eigen::Vector2d & position,
                       double angle,
                       const Vector2dList & corners)
	: d_reference(reference)
	, d_URI(d_reference.URI() / "closeups" / std::to_string(tid))
	, d_absoluteFilePath(absoluteFilePath)
	, d_tagID(tid)
	, d_tagPosition(position)
	, d_tagAngle(angle)
	, d_corners(corners) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);
}

TagCloseUp::~TagCloseUp() {}

const FrameReference & TagCloseUp::Frame() const {
	return d_reference;
}

const fs::path & TagCloseUp::URI() const {
	return d_URI;
}

const fs::path & TagCloseUp::AbsoluteFilePath() const  {
	return d_absoluteFilePath;
}

TagID TagCloseUp::TagValue() const {
	return d_tagID;
}

const Eigen::Vector2d & TagCloseUp::TagPosition() const {
	return d_tagPosition;
}

double TagCloseUp::TagAngle() const {
	return d_tagAngle;
}

const TagCloseUp::Vector2dList & TagCloseUp::Corners() const {
	return d_corners;
}



} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagCloseUp & p) {
	return out << p.Frame() << "/closeups/" << p.TagValue();
}
