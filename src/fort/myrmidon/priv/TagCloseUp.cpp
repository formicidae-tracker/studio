#include "TagCloseUp.hpp"

#include <regex>

#include <apriltag/tag16h5.h>
#include <apriltag/tag25h9.h>
#include <fort/tags/tag36h10.h>
#include <apriltag/tag36h11.h>
#include <fort/tags/tag36ARTag.h>
#include <apriltag/tagCircle21h7.h>
#include <apriltag/tagCircle49h12.h>
#include <apriltag/tagCustom48h12.h>
#include <apriltag/tagStandard41h12.h>
#include <apriltag/tagStandard52h13.h>


#include <fort/myrmidon/utils/Checker.hpp>
#include <fort/myrmidon/priv/proto/FileReadWriter.hpp>
#include <fort/myrmidon/priv/proto/IOUtils.hpp>
#include <fort/myrmidon/TagCloseUpCache.pb.h>

#include <fort/myrmidon/utils/Defer.hpp>

#include <opencv2/imgcodecs.hpp>

#include <iostream>

namespace fort {
namespace myrmidon {
namespace priv {


double TagCloseUp::ComputeAngleFromCorners(const Eigen::Vector2d & c0,
                                           const Eigen::Vector2d & c1,
                                           const Eigen::Vector2d & c2,
                                           const Eigen::Vector2d & c3) {
	Eigen::Vector2d delta = (c1 + c2) / 2.0 - (c0 + c3) / 2.0;
	return atan2(delta.y(),delta.x());
}


std::string TagCloseUp::FormatURI(const std::string & tddURI,
                                  FrameID frameID,
                                  TagID tagID) {
	return (fs::path(tddURI) / "frames" / std::to_string(frameID) /  "closeups" / FormatTagID(tagID)).generic_string();
}


TagCloseUp::TagCloseUp(const fs::path & absoluteFilePath,
                       const FrameReference & reference,
                       TagID tagID,
                       const Eigen::Vector2d & position,
                       double angle,
                       const Vector2dList & corners)
	: d_reference(reference)
	, d_URI(FormatURI(reference.ParentURI(),reference.FrameID(),tagID))
	, d_absoluteFilePath(absoluteFilePath)
	, d_tagID(tagID)
	, d_tagPosition(position)
	, d_tagAngle(angle)
	, d_corners(corners) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);
	if (corners.size() != 4 ) {
		throw std::invalid_argument("A tag needs 4 corners, only got " + std::to_string(corners.size()));
	}
}

TagCloseUp::TagCloseUp(const fs::path & absoluteFilePath,
                       const FrameReference & reference,
                       const apriltag_detection_t * d)
	: d_reference(reference)
	, d_URI( FormatURI(reference.ParentURI(),reference.FrameID(),d->id) )
	, d_absoluteFilePath(absoluteFilePath)
	, d_tagID(d->id)
	, d_tagPosition(d->c[0],d->c[1])
	, d_tagAngle(0.0)
	, d_corners(4) {
	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);
	for(size_t i = 0; i < 4; ++i ) {
		d_corners[i] << d->p[i][0],d->p[i][1];
	}
	d_tagAngle = ComputeAngleFromCorners(d_corners[0],
	                                     d_corners[1],
	                                     d_corners[2],
	                                     d_corners[3]);
}

TagCloseUp::~TagCloseUp() {}

const FrameReference & TagCloseUp::Frame() const {
	return d_reference;
}

const std::string & TagCloseUp::URI() const {
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

const Vector2dList & TagCloseUp::Corners() const {
	return d_corners;
}

Isometry2Dd TagCloseUp::ImageToTag() const {
	return Isometry2Dd(d_tagAngle,d_tagPosition).inverse();
}

double TagCloseUp::TagSizePx() const {
	double res = (d_corners[0] - d_corners[1]).norm()
		+ (d_corners[1] - d_corners[2]).norm()
		+ (d_corners[2] - d_corners[3]).norm()
		+ (d_corners[3] - d_corners[0]).norm();

	return res / 4.0;
}

double TagCloseUp::Squareness() const {
	double maxAngleDistanceToPI_2 = 0.0;
	for(size_t i = 0 ; i < 4; ++i ) {
		Eigen::Vector2d a = d_corners[(i-1)%4] - d_corners[i];
		Eigen::Vector2d b = d_corners[(i+1)%4] - d_corners[i];
		double aNorm = a.norm();
		double bNorm = b.norm();
		if ( aNorm < 1.0e-3 || bNorm < 1.0e-3 ) {
			return 0;
		}
		double angle = std::acos(a.dot(b)/ (aNorm * bNorm));
		maxAngleDistanceToPI_2 = std::max(maxAngleDistanceToPI_2,
		                                  std::abs(angle - (M_PI / 2.0)));
	}
	return 1.0 - maxAngleDistanceToPI_2 / M_PI * 2.0;
}

} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagCloseUp & p) {
	return out << p.Frame() << "/closeups/"
	           << fort::myrmidon::FormatTagID(p.TagValue());
}
