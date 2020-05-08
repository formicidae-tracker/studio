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
