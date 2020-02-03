#pragma once

#include <Eigen/Core>

#include <fort-tags/fort-tags.h>

#include "FrameReference.hpp"


typedef struct apriltag_detection apriltag_detection_t;

namespace fort {
namespace myrmidon {
namespace priv {


class TagCloseUp : public Identifiable , public FileSystemLocatable {
public:
	typedef std::shared_ptr<TagCloseUp>       Ptr;
	typedef std::shared_ptr<const TagCloseUp> ConstPtr;

	typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d> > Vector2dList;

	typedef std::function<ConstPtr()> Loader;
	typedef std::map<fs::path,ConstPtr> TagCloseUpByURI;

	static std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>> ListFiles(const fs::path & absoluteFilePath);

	static std::vector<Loader> PrepareLoading(const fs::path & absoluteFilePath,
	                                          fort::tags::Family f,
	                                          uint8_t threshold);

	static TagCloseUpByURI Load(const fs::path & absoluteFilePath,
	                            fort::tags::Family f,
	                            uint8_t threshold);

	TagCloseUp(const fs::path & absoluteFilePath,
	           const FrameReference & reference,
	           TagID tid,
	           const Eigen::Vector2d & position,
	           double angle,
	           const Vector2dList & corners);

	virtual ~TagCloseUp();


	const FrameReference & Frame() const;


	const fs::path & URI() const override;

	const fs::path & AbsoluteFilePath() const override;

	TagID TagValue() const;
	const Eigen::Vector2d & TagPosition() const;
	double TagAngle() const;
	const Vector2dList & Corners() const;




private:
	FrameReference  d_reference;
	fs::path        d_URI;
	fs::path        d_absoluteFilePath;
	TagID           d_tagID;
	Eigen::Vector2d d_tagPosition;
	double          d_tagAngle;
	Vector2dList    d_corners;
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
