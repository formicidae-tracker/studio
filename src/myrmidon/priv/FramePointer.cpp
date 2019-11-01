#include "FramePointer.hpp"

#include <sstream>

#include "../utils/NotYetImplemented.hpp"

namespace fm = fort::myrmidon;

using namespace fort::myrmidon::priv;


fs::path FramePointer::FullPath() const {
	std::ostringstream os;
	os << Frame;
	return Path / os.str();
}


const fm::Time & FramePointer::Time() const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}
