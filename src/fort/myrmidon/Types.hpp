#pragma once

#include <variant>
#include "Time.hpp"

namespace fort {
namespace myrmidon {

// The ID for a tag
typedef uint32_t TagID;

// A named value
//
// AntStaticValue holds a value for a <named_values>.
typedef std::variant<bool,int32_t,double,std::string,Time> AntStaticValue;



}
}


// C++ Formatting operator for AntStaticValue
// @out the <std::ostream> to format the value to
// @color the <fort::myrmidon::AntStaticValue> to format
//
// @return a reference to <out>

std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue &);
