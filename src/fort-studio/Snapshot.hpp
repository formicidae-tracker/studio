#pragma once


#include <filesystem>

#include <Eigen/StdVector>

#include "Snapshot.pb.h"

#include <apriltag.h>

class Snapshot {
public:
	typedef std::shared_ptr<const Snapshot> ConstPtr;
	typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d> > Vector2dList;

	uint64_t Frame() const;

	uint32_t TagValue();
	Eigen::Vector2d TagPosition() const;
	double TagAngle() const;

	const Vector2dList & Corners() const;


	std::filesystem::path ImagePath() const;

	void Encode(fort::myrmidon::pb::Snapshot & pb) const;
	static ConstPtr FromSaved(const fort::myrmidon::pb::Snapshot & pb, const std::filesystem::path & basedir);
	static ConstPtr FromApriltag(const apriltag_detection_t * d,
	                             const std::filesystem::path & imagePath,
	                             const std::filesystem::path & basedir,
	                             uint64_t frame);


	std::filesystem::path Path() const;

private:
	Eigen::Vector3d       d_position;
	uint32_t              d_value;
	uint64_t              d_frame;
	Vector2dList          d_corners;
	std::filesystem::path d_relativeImagePath;
	std::filesystem::path d_basedir;
};
