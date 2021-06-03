#include "Types.hpp"

#include "priv/Measurement.hpp"

namespace fort {
namespace myrmidon {

bool IdentifiedFrame::Contains(uint64_t antID) const {
	return (Positions.array().col(0) == double(antID)).any();
}

Time AntTrajectory::End() const {
	if ( Positions.rows() == 0 ) {
		return Start;
	}
	return Start.Add(Positions(Positions.rows()-1,0) * Duration::Second);
}

std::string FormatTagID(TagID tagID) {
	std::ostringstream oss;
	oss << "0x" << std::hex << std::setfill('0') << std::setw(3) << tagID;
	return oss.str();
}

} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue & v) {
	auto flags = out.flags();
	std::visit([&out](auto && args) { out << std::boolalpha << args; },v);
	out.flags(flags);
	return out;
}
