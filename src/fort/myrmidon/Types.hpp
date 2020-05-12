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



}
}


// C++ Formatting operator for AntStaticValue
// @out the <std::ostream> to format the value to
// @v the <fort::myrmidon::AntStaticValue> to format
//
// @return a reference to <out>
std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue & v);
