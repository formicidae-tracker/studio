#include "Types.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

bool IdentifiedFrame::Contains(uint64_t antID) const {
	return std::find_if(Positions.cbegin(),
	                    Positions.cend(),
	                    [antID](const PositionedAnt & ant) {
		                    return antID == ant.ID;
	                    }) != Positions.cend();
}

} // namespace priv
} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out, const fort::myrmidon::priv::AntStaticValue & v) {
	auto flags = out.flags();
	std::visit([&out](auto && args) { out << std::boolalpha << args; },v);
	out.flags(flags);
	return out;
}
