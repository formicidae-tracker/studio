#include "Handle.hpp"


#include <QPen>

const int Handle::SIZE = 5;
const QColor Handle::COLOR = QColor(50,50,50,120);
const QColor Handle::SELECTED_COLOR = QColor(150,150,150,120);


Handle::Handle(MovedCallback onMove,
               ReleasedCallback onRelease,
               QGraphicsItem * parent)
	: QGraphicsItemGroup(parent)
	, d_onMove(onMove)
	, d_onRelease(onRelease) {
	const static int HALF_SIZE = (SIZE-1)/2;
	QPen empty;
	empty.setStyle(Qt::NoPen);
	d_inside = new QGraphicsRectItem(-HALF_SIZE-0.5,
	                                 -HALF_SIZE-0.5,
	                                 SIZE,
	                                 SIZE,this);
	d_inside->setPen(QPen(QColor(255,255,255),1));
	d_inside->setBrush(COLOR);
	setFlags(QGraphicsItem::ItemIsMovable);
}

Handle::~Handle() {

}

void Handle::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItemGroup::mouseMoveEvent(e);
	d_onMove();
}

void Handle::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItemGroup::mouseReleaseEvent(e);
	d_onRelease();
}
