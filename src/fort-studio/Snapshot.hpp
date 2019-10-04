#pragma once


#include <filesystem>

#include <Eigen/StdVector>

#include <apriltag.h>

#include <myrmidon/priv/TrackingDataDirectory.hpp>

class Snapshot {
public:
	typedef std::shared_ptr<const Snapshot> ConstPtr;
	typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d> > Vector2dList;

	fort::myrmidon::priv::FramePointerPtr Frame() const;

	uint32_t TagValue() const;
	Eigen::Vector2d TagPosition() const;
	double TagAngle() const;

	const Vector2dList & Corners() const;


	std::filesystem::path ImagePath() const;

	static ConstPtr FromApriltag(const apriltag_detection_t * d,
	                             const std::filesystem::path & relativeImagePath,
	                             const fort::myrmidon::priv::FramePointerPtr & frame);


	std::filesystem::path Path() const;

	const std::filesystem::path & Base() const;

private:


	Eigen::Vector3d       d_position;
	uint32_t              d_value;
	Vector2dList          d_corners;
	std::filesystem::path d_relativeImagePath;

	fort::myrmidon::priv::FramePointerPtr d_frame;

};
