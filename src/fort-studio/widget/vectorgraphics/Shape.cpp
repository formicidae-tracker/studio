#include "Shape.hpp"


const qreal Shape::LINE_WIDTH = 1.5;
const int   Shape::BORDER_OPACITY = 200;
const int   Shape::FILL_OPACITY = 40;


Shape::Shape(const QColor & color,
             QObject * parent)
	: QObject(parent)
	, d_color(color) {
}

Shape::~Shape() {
}

void Shape::setColor(const QColor & color) {
	d_color = color;
}
