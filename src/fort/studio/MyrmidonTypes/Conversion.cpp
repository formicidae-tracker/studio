#include "Conversion.hpp"

QColor Conversion::colorFromFM(const fm::Color & color, int opacity) {
	return QColor(std::get<0>(color),
	              std::get<1>(color),
	              std::get<2>(color),
	              opacity);
}

QIcon Conversion::iconFromFM(const fm::Color & color) {
	QPixmap icon(15,15);
	icon.fill(colorFromFM(color));
	return icon;
}


QPointF Conversion::fromEigen(const Eigen::Vector2d & p) {
	return QPointF(p.x(),p.y());
}

Eigen::Vector2d Conversion::toEigen(const QPointF  & p) {
	return Eigen::Vector2d(p.x(),p.y());
}
