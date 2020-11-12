#pragma once

#include <Eigen/Core>

#include <fort/tags/fort-tags.hpp>
#include <fort/tags/options.hpp>


#include "FrameReference.hpp"
#include "Isometry2D.hpp"
#include <mutex>

typedef struct apriltag_detection apriltag_detection_t;

namespace fort {
namespace myrmidon {
namespace priv {


class TagCloseUp : public Identifiable , public FileSystemLocatable {
public:
	typedef std::shared_ptr<TagCloseUp>       Ptr;
	typedef std::shared_ptr<const TagCloseUp> ConstPtr;
	typedef std::vector<ConstPtr>             List;



	static double ComputeAngleFromCorners(const Eigen::Vector2d & c0,
	                                      const Eigen::Vector2d & c1,
	                                      const Eigen::Vector2d & c2,
	                                      const Eigen::Vector2d & c3);

	static std::string FormatURI(const std::string & tddURI,
	                             FrameID frameID,
	                             TagID tagID);

	TagCloseUp(const fs::path & absoluteFilePath,
	           const FrameReference & reference,
	           TagID tid,
	           const Eigen::Vector2d & position,
	           double angle,
	           const Vector2dList & corners);

	TagCloseUp(const fs::path & absoluteFilePath,
	           const FrameReference & reference,
	           const apriltag_detection_t * d);


	virtual ~TagCloseUp();


	const FrameReference & Frame() const;


	const std::string & URI() const override;

	const fs::path & AbsoluteFilePath() const override;

	TagID TagValue() const;
	const Eigen::Vector2d & TagPosition() const;
	double TagAngle() const;
	const Vector2dList & Corners() const;

	Isometry2Dd ImageToTag() const;

	double TagSizePx() const;

	double Squareness() const;

private:
	FrameReference  d_reference;
	std::string     d_URI;
	fs::path        d_absoluteFilePath;
	TagID           d_tagID;
	Eigen::Vector2d d_tagPosition;
	double          d_tagAngle;
	Vector2dList    d_corners;
	double          d_tagWidthPx,d_squareness;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


} // namespace priv
} // namespace myrmidon
} // namespace fort

// Formats a TagInFrameReference
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::TagInFrameReference> to format
// @return a reference to <out>
std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagCloseUp & tcu);
