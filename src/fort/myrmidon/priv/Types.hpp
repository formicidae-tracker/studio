#pragma once

#include <fort/myrmidon/Types.hpp>

#include <cstdint>

#include <tuple>
#include <variant>
#include <string>

#include <unordered_map>
#include <vector>

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

// AABB
typedef Eigen::AlignedBox<double,2> AABB;


typedef std::pair<Time,AntStaticValue>   AntTimedValue;

typedef std::unordered_map<std::string,std::vector<AntTimedValue> > AntDataMap;
typedef std::unordered_map<std::string,const std::vector<AntTimedValue> > AntConstDataMap;

typedef Eigen::Ref<Eigen::Vector2d> Vector2dRef;

typedef Eigen::Ref<Eigen::Matrix<double,1,5,Eigen::RowMajor>>       PositionedAntRef;
typedef Eigen::Ref<const Eigen::Matrix<double,1,5,Eigen::RowMajor>> PositionedAntConstRef;





} // namespace priv
} // namespace myrmidon
} // namespace fort
