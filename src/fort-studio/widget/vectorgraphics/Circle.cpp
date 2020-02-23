#include "Circle.hpp"

#include <QGraphicsScene>

#include "Handle.hpp"

#include <fort-studio/Utils.hpp>




Circle::Circle(const QPointF & pos,
               qreal radius,
               QColor color,
               QGraphicsItem * parent)
	: Shape(color,nullptr)
	, QGraphicsEllipseItem(parent)
	, d_radius(radius) {

	setRect(pos.x() - radius,
	        pos.y() - radius,
	        2 * radius,
	        2 * radius);

	d_center = new Handle([this] () { update(true); },
	                      [this] () { update(true); emit updated(); });
	d_center->setPos(pos);

	d_radiusHandle = new Handle([this] () { update(false); },
	                            [this] () { update(false); emit updated(); });

	d_radiusHandle->setPos(pos.x() + radius,pos.y());

	setFlags(QGraphicsItem::ItemIsSelectable);
}

Circle::~Circle() {
}


void Circle::addToScene(QGraphicsScene * scene) {
	scene->addItem(this);
	scene->addItem(d_center);
	scene->addItem(d_radiusHandle);
}



void Circle::paint(QPainter * painter,
                   const QStyleOptionGraphicsItem * option,
                   QWidget * widget) {

	QColor actual = d_color;
	if ( isSelected() == true ) {
		actual = d_color.lighter(150);
	}
	actual.setAlpha(BORDER_OPACITY);
	setPen(QPen(actual,LINE_WIDTH));
	actual.setAlpha(FILL_OPACITY);
	setBrush(actual);
	QGraphicsEllipseItem::paint(painter,option,widget);

}

void Circle::update(bool fixRadius) {
	auto c = d_center->pos();
	if ( fixRadius == false ) {
		d_radius = (ToEigen(c) - ToEigen(d_radiusHandle->pos())).norm();
	}
	d_radiusHandle->setPos(c.x() + d_radius,c.y());
	setRect(c.x() - d_radius,
	        c.y() - d_radius,
	        2*d_radius,
	        2*d_radius);
}


QPointF Circle::pos() const {
	return d_center->pos();
}

qreal Circle::radius() const {
	return d_radius;
}
