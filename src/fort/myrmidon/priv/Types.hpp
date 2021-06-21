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



typedef std::pair<Time,AntStaticValue>   AntTimedValue;

typedef std::unordered_map<std::string,std::vector<AntTimedValue> > AntDataMap;
typedef std::unordered_map<std::string,const std::vector<AntTimedValue> > AntConstDataMap;

typedef Eigen::Ref<Eigen::Vector2d> Vector2dRef;

typedef Eigen::Ref<IdentifiedFrame::PositionMatrix>       PositionedAntRef;
typedef Eigen::Ref<const IdentifiedFrame::PositionMatrix> PositionedAntConstRef;





} // namespace priv
} // namespace myrmidon
} // namespace fort
