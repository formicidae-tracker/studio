#pragma once

#include <variant>
#include "Time.hpp"

namespace fort {
namespace myrmidon {

typedef uint32_t TagID;

typedef std::variant<bool,int32_t,double,std::string,Time> AntStaticValue;



}
}

std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue &);
