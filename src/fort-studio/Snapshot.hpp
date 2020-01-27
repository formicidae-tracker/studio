#pragma once


#include <myrmidon/utils/FileSystem.hpp>

#include <Eigen/StdVector>

#include <apriltag/apriltag.h>

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include "TagInFramePointer.hpp"

class Snapshot : public TagInFramePointer {
public:
	typedef std::shared_ptr<const Snapshot> ConstPtr;
	typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d> > Vector2dList;

	Snapshot(const fort::myrmidon::priv::RawFrame::ConstPtr & frame,
	         fort::myrmidon::priv::TagID tagValue);
	virtual ~Snapshot();

	Eigen::Vector2d TagPosition() const;
	double TagAngle() const;

	const Vector2dList & Corners() const;


	fs::path ImagePath() const;

	static ConstPtr FromApriltag(const apriltag_detection_t * d,
	                             const fs::path & relativeImagePath,
	                             const fort::myrmidon::priv::RawFrame::ConstPtr & frame);


private:
	friend class SnapshotIndexer;

	Eigen::Vector3d       d_position;
	Vector2dList          d_corners;
	fs::path              d_relativeImagePath;

};
