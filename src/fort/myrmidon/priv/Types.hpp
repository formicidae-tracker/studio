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


typedef std::pair<Time::ConstPtr,AntStaticValue>   AntTimedValue;

typedef std::unordered_map<std::string,std::vector<AntTimedValue> > AntDataMap;
typedef std::unordered_map<std::string,const std::vector<AntTimedValue> > AntConstDataMap;


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
	bool Contains(uint64_t antID) const;
};


typedef std::pair<uint32_t,uint32_t> InteractionType;
typedef std::pair<uint32_t,uint32_t> InteractionID;

struct Interaction {
	InteractionID                IDs;
	std::vector<InteractionType> InteractionTypes;
};

struct InteractionFrame {
	typedef std::shared_ptr<const InteractionFrame> ConstPtr;
	Time                     FrameTime;
	std::vector<Interaction> Interactions;
};




} // namespace priv
} // namespace myrmidon
} // namespace fort
