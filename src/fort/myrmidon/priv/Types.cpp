#include "Types.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

std::string FormatTagID(TagID tagID) {
	std::ostringstream oss;
	oss << "0x" << std::hex << std::setfill('0') << std::setw(3) << tagID;
	return oss.str();
}



} // namespace priv
} // namespace myrmidon
} // namespace fort
