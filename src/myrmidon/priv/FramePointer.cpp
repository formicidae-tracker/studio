#include "FramePointer.hpp"

#include <sstream>

using namespace fort::myrmidon::priv;


fs::path FramePointer::FullPath() const {
	std::ostringstream os;
	os << Frame;
	return Path / os.str();
}
