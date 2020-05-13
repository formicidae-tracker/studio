#pragma once

#include <variant>
#include <map>
#include <vector>

#include <Eigen/Core>

#include "Time.hpp"


#include "ForwardDeclaration.hpp"



namespace fort {
namespace myrmidon {

// The ID for a tag
//
// As explained in <tag_identification> myrmidon doesn't allows to
// access tracking tag data directly, but through the definition of
// <Ant> and <Identification>.
typedef uint32_t TagID;

// A named value
//
// AntStaticValue holds a value for a <named_values>.
typedef std::variant<bool,int32_t,double,std::string,Time> AntStaticValue;

// A List of 2D Vector
typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d>> Vector2dList;

// ID for Ant virtual body parts
//
// ID to designate an <Ant> virtual body part. See
// <ant_collision_detection>
typedef uint32_t AntShapeTypeID;

// ID for manual measurements
//
// ID to designate a manual measurement made in the GUI
typedef uint32_t MeasurementTypeID;

// Mandatory measurement type for any <Experiment>
//
// Any <Experiment> have this Measurement type that cannot be removed.
const MeasurementTypeID HEAD_TAIL_MEASUREMENT_TYPE = 1;


// A list of Ant virtual shape part
typedef std::vector<std::pair<AntShapeTypeID,CapsuleConstPtr>>                 TypedCapsuleList;

// Types for non-tracking data column
enum class AntMetadataType {
                            // A boolean
                            Bool = 0,
                            // An integer
                            Int,
                            // a float
                            Double,
                            // a std::string
                            String,
                            // a <Time>
                            Time,
};

// Represents a Measurement in mm at a given Time.
struct ComputedMeasurement {
	typedef std::vector<ComputedMeasurement> List;
	Time   MTime;
	double LengthMM;
};

struct TagStatistics {
	typedef std::map<TagID,TagStatistics>   ByTagID;

	enum CountHeader {
		TOTAL_SEEN        = 0,
		MULTIPLE_SEEN     = 1,
		GAP_500MS         = 2,
		GAP_1S            = 3,
		GAP_10S           = 4,
		GAP_1M            = 5,
		GAP_10M           = 6,
		GAP_1H            = 7,
		GAP_10H           = 8,
		GAP_MORE          = 9,
	};

	TagID    ID;
	Time     FirstSeen,LastSeen;
	Eigen::Matrix<uint64_t,Eigen::Dynamic,1> Counts;

};


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


typedef std::pair<AntID,AntID>                   InteractionID;
typedef std::pair<AntShapeTypeID,AntShapeTypeID> InteractionType;

struct PonctualInteraction {
	InteractionID                IDs;
	std::vector<InteractionType> InteractionTypes;
	ZoneID                       Zone;
};

struct InteractionFrame {
	typedef std::shared_ptr<const InteractionFrame> ConstPtr;
	Time                             FrameTime;
	std::vector<PonctualInteraction> Interactions;
};


struct AntTrajectory {
	typedef std::shared_ptr<const AntTrajectory> ConstPtr;

	AntID   Ant;
	SpaceID Space;
	Time    Start;

	Eigen::Matrix<double,Eigen::Dynamic,3> Positions;
	std::vector<uint64_t>                  Nanoseconds;
};

struct AntInteraction {
	typedef std::shared_ptr<const AntInteraction> ConstPtr;

	InteractionID                          IDs;
	std::pair<AntTrajectory,AntTrajectory> Trajectories;
	Time                                   Start,End;
};

}
}


// C++ Formatting operator for AntStaticValue
// @out the <std::ostream> to format the value to
// @v the <fort::myrmidon::AntStaticValue> to format
//
// @return a reference to <out>
std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue & v);
