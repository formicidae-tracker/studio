#pragma once


#include <filesystem>

#include <Eigen/StdVector>

#include <apriltag.h>

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include "TagInFramePointer.hpp"

class Snapshot : public TagInFramePointer {
public:
	typedef std::shared_ptr<const Snapshot> ConstPtr;
	typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d> > Vector2dList;

	Snapshot(const fort::myrmidon::priv::FramePointer::Ptr & frame,
	         uint32_t tagValue);
	virtual ~Snapshot();

	Eigen::Vector2d TagPosition() const;
	double TagAngle() const;

	const Vector2dList & Corners() const;


	std::filesystem::path ImagePath() const;

	static ConstPtr FromApriltag(const apriltag_detection_t * d,
	                             const std::filesystem::path & relativeImagePath,
	                             const fort::myrmidon::priv::FramePointerPtr & frame);


private:
	friend class SnapshotIndexer;

	Eigen::Vector3d       d_position;
	Vector2dList          d_corners;
	std::filesystem::path d_relativeImagePath;

};
