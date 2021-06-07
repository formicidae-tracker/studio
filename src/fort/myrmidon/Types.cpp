#include "Types.hpp"

#include "priv/Measurement.hpp"

namespace fort {
namespace myrmidon {

bool IdentifiedFrame::Contains(uint64_t antID) const {
	return (Positions.array().col(0) == double(antID)).any();
}

std::tuple<AntID,const Eigen::Ref<const Eigen::Vector3d>,ZoneID> IdentifiedFrame::At(size_t index) const {
	if ( index > Positions.rows() ) {
		throw std::out_of_range(std::to_string(index) + " is out of range [0," + std::to_string(Positions.rows()) + "[");
	}
	AntID antID = AntID(Positions(index,0));
	ZoneID zoneID = ZoneID(Positions(index,4));
	Eigen::Ref<const Eigen::Vector3d> position = Positions.block<1,3>(index,1).transpose();
	return std::make_tuple(antID,position,zoneID);
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
