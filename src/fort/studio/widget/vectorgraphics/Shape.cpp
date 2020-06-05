#include "Shape.hpp"

#include <QGraphicsScene>

const qreal Shape::LINE_WIDTH = 2.5;
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
	emit colorUpdated();
}


void Shape::addToScene( QGraphicsScene * scene ) {
	connect(this,SIGNAL(colorUpdated()),
	        scene,SLOT(update()));
	addToSceneProtected(scene);
}
void Shape::removeFromScene( QGraphicsScene * scene ) {
	removeFromSceneProtected(scene);
	disconnect(this,SIGNAL(colorUpdated()),
	           scene,SLOT(update()));

}
