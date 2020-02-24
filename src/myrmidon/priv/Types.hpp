#pragma once


#include <cstdint>

#include <tuple>

#include <Eigen/Geometry>

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



} // namespace priv
} // namespace myrmidon
} // namespace fort
