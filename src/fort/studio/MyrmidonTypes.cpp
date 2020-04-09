#include "MyrmidonTypes.hpp"


QColor Conversion::colorFromFM(const fmp::Color & color, int opacity) {
	return QColor(std::get<0>(color),
	              std::get<1>(color),
	              std::get<2>(color),
	              opacity);
}
