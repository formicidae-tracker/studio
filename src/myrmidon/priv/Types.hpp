#pragma once


#include <cstdint>

#include <tuple>

#include <Eigen/Geometry>

#include <myrmidon/Time.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

// The ID of a frame
typedef uint64_t FrameID;

// The ID for a movie frame
typedef uint64_t MovieFrameID;

// The ID for a tag
typedef uint32_t TagID;

// A List of 2D vector
typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d>> Vector2dList;

// AABB
typedef Eigen::AlignedBox<double,2> AABB;

struct PositionedAnt {
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	Eigen::Vector2d Position;
	double          Angle;
	// here Ant::ID is a 32 bits, but will break the 16-bytes alignemnet of this structure.
	uint64_t        ID;
};

typedef std::vector<PositionedAnt,Eigen::aligned_allocator<PositionedAnt>> PositionedAntList;

struct IdentifiedFrame {
	typedef std::shared_ptr<const IdentifiedFrame> ConstPtr;
	Time              FrameTime;
	size_t            Height;
	size_t            Width;
	PositionedAntList Positions;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
