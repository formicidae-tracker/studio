#include "Vector.hpp"

#include <QPen>

#include <Eigen/Core>

#include <iostream>

#include "VectorialScene.hpp"

const qreal Vector::LINE_WIDTH = 1.5;
const int   Vector::OPACITY = 255;
const double Vector::ARROW_LENGTH = 15;
const double Vector::ARROW_WIDTH = 6;
const qreal Vector::PrecisionHandle::SIZE = 15;
const qreal Vector::PrecisionHandle::LINE_WIDTH = 0.8;


Vector::Vector(qreal ax, qreal ay,
               qreal bx, qreal by,
               QColor color,
               UpdatedCallback onUpdated,
               VectorialScene * scene,
               QGraphicsItem * parent)
	: QGraphicsItemGroup(parent)
	, d_onUpdated(onUpdated)
	, d_color(color) {
	d_line = new QGraphicsPathItem(this);

	d_start =
		new PrecisionHandle([this]() {
			                    rebuild();
		                    },
			[this]() {
				rebuild();
				editFinished();
			});


	d_end =
		new PrecisionHandle([this]() {
			                    rebuild();
		                    },
			[this]() {
				rebuild();
				editFinished();
			});

	d_start->setPos(ax,ay);
	d_end->setPos(bx,by);

	scene->addItem(d_start);
	scene->addItem(d_end);



	setFlags(QGraphicsItem::ItemIsSelectable);
	rebuild();
}

Vector::~Vector() {
}

void Vector::setColor(const QColor & color) {
	d_color = color;
}

void Vector::paint(QPainter * painter,
                   const QStyleOptionGraphicsItem * option,
                   QWidget * widget) {
	QColor actual = d_color;

	if ( isSelected() ) {
		actual = d_color.lighter(150);
	}
	actual.setAlpha(255);
	d_line->setPen(QPen(actual,LINE_WIDTH));
	d_start->setColor(actual);
	d_end->setColor(actual);
	QGraphicsItemGroup::paint(painter,option,widget);

}

Vector::PrecisionHandle::PrecisionHandle(MovedCallback onMove,
                                         ReleasedCallback onRelease,
                                         QGraphicsItem * parent)
	: QGraphicsItemGroup(parent)
	, d_onMove(onMove)
	, d_onRelease(onRelease) {

	d_circle = new QGraphicsEllipseItem(-SIZE,-SIZE,2*SIZE,2*SIZE,this);
#define SQRT_2_2 0.70710678118
#define RATIO 0.3
	d_lines[0] = new QGraphicsLineItem(-1.0 * SQRT_2_2 * SIZE,  -1.0 *SQRT_2_2 * SIZE,
	                                   -RATIO * SQRT_2_2 * SIZE,-RATIO *SQRT_2_2 * SIZE,
	                                   this);

	d_lines[1] = new QGraphicsLineItem(+1.0 * SQRT_2_2 * SIZE,  -1.0 *SQRT_2_2 * SIZE,
	                                   +RATIO * SQRT_2_2 * SIZE,-RATIO *SQRT_2_2 * SIZE,
	                                   this);

	d_lines[2] = new QGraphicsLineItem(+1.0 * SQRT_2_2 * SIZE,  +1.0 *SQRT_2_2 * SIZE,
	                                   +RATIO * SQRT_2_2 * SIZE,+RATIO *SQRT_2_2 * SIZE,
	                                   this);

	d_lines[3] = new QGraphicsLineItem(-1.0 * SQRT_2_2 * SIZE,  +1.0 *SQRT_2_2 * SIZE,
	                                   -RATIO * SQRT_2_2 * SIZE,+RATIO *SQRT_2_2 * SIZE,
	                                   this);

	setFlags(QGraphicsItem::ItemIsMovable);

}

Vector::PrecisionHandle::~PrecisionHandle() {

}


void Vector::PrecisionHandle::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItemGroup::mouseMoveEvent(e);
	d_onMove();
}

void Vector::PrecisionHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItemGroup::mouseReleaseEvent(e);
	d_onRelease();
}

void Vector::PrecisionHandle::setColor(const QColor & color) {
	d_circle->setPen(QPen(color,LINE_WIDTH));
	d_circle->setBrush(QColor(color.red(),color.green(),color.blue(),10));
	for ( size_t i = 0 ; i < 4; ++i ) {
		d_lines[i]->setPen(QPen(color,LINE_WIDTH));
	};
}

void Vector::rebuild() {
	Eigen::Vector2d start(d_start->x(),d_start->y());
	Eigen::Vector2d end(d_end->x(),d_end->y());
	Eigen::Vector2d diff = end-start;
	auto dist = diff.norm();

	if ( dist < ARROW_LENGTH + PrecisionHandle::SIZE) {
		d_line->setPath(QPainterPath());
		return;
	}
	diff /= dist;

	start += diff * PrecisionHandle::SIZE;
	end -= diff * PrecisionHandle::SIZE;

	Eigen::Vector2d ortho(diff.y(),-diff.x());
	Eigen::Vector2d arrowStart = start + 0.66 * (dist - PrecisionHandle::SIZE) * diff;
	Eigen::Vector2d arrowA = arrowStart-diff*ARROW_LENGTH;
	Eigen::Vector2d arrowB = arrowA + ARROW_WIDTH * ortho;
	arrowA -= ARROW_WIDTH * ortho;

	QPainterPath path;
	path.moveTo(start.x(),start.y());
	path.lineTo(end.x(),end.y());
	path.moveTo(arrowStart.x(),arrowStart.y());
	path.lineTo(arrowA.x(),arrowA.y());
	path.moveTo(arrowB.x(),arrowB.y());
	path.lineTo(arrowStart.x(),arrowStart.y());

	d_line->setPath(path);
}

void Vector::editFinished() {
	d_onUpdated(d_start->pos(),d_end->pos());
}
