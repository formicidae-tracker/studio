#pragma once

#include <QObject>
#include <QColor>
#include <QIcon>

#include <Eigen/Core>

#include "Color.hpp"

namespace fm = fort::myrmidon;

class Conversion {
public:
	static QColor colorFromFM(const fm::Color & color, int opacity = 255);
	static QIcon  iconFromFM(const fm::Color & color);
	static QPointF fromEigen(const Eigen::Vector2d & p);
	static Eigen::Vector2d toEigen(const QPointF  & p);
};
