#include "Types.hpp"


bool operator==(const fort::myrmidon::priv::AntStaticValue & a,
                const fort::myrmidon::priv::AntStaticValue & b) {
	if ( a.valueless_by_exception() == true ) {
		return b.valueless_by_exception();
	}
	if ( b.valueless_by_exception() == true ) {
		return false;
	}

	if  ( a.index() != b.index() ) {
		return false;
	}

	switch(a.index()) {
	case 0:
		return std::get<bool>(a) == std::get<bool>(b);
	case 1:
		return std::get<int32_t>(a) == std::get<int32_t>(b);
	case 2:
		return std::get<double>(a) == std::get<double>(b);
	case 3:
		return std::get<std::string>(a) == std::get<std::string>(b);
	case 4:
		return std::get<fort::myrmidon::Time>(a) == std::get<fort::myrmidon::Time>(b);
	default:
		throw std::logic_error("Unknown type");
	}
}
