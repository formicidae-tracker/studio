#include "MyrmidonTypes.hpp"


QColor Conversion::colorFromFM(const fmp::Color & color, int opacity) {
	return QColor(std::get<0>(color),
	              std::get<1>(color),
	              std::get<2>(color),
	              opacity);
}

QIcon Conversion::iconFromFM(const fmp::Color & color) {
	QPixmap icon(15,15);
	icon.fill(colorFromFM(color));
	return icon;
}
