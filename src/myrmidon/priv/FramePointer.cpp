#include "FramePointer.hpp"



using namespace fort::myrmidon::priv;


std::filesystem::path FramePointer::FullPath() const {
	std::ostringstream os;
	os << Frame;
	return Path / os.str();
}
