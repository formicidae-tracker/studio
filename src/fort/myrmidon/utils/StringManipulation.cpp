#include "StringManipulation.hpp"


namespace fort {
namespace myrmidon {
namespace utils {

bool HasPrefix(const std::string & s, const std::string & prefix ) {
	if (prefix.size() > s.size() ) {
		return false;
	}
	return std::mismatch(prefix.begin(),prefix.end(),s.begin(),s.end()).first == prefix.end();
}

} // namespace utils
} // namespace myrmidon
} // namespace fort
