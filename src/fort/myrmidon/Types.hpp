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

typedef std::vector<std::pair<AntShapeTypeID,CapsuleConstPtr>>                 TypedCapsuleList;

}
}


// C++ Formatting operator for AntStaticValue
// @out the <std::ostream> to format the value to
// @v the <fort::myrmidon::AntStaticValue> to format
//
// @return a reference to <out>
std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue & v);
