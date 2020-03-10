#include "Vector.hpp"

#include <QPen>

#include <Eigen/Core>

#include "VectorialScene.hpp"
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

const double Vector::ARROW_LENGTH = 16;
const double Vector::ARROW_WIDTH = 8;
const qreal Vector::PrecisionHandle::SIZE = 15;
const qreal Vector::PrecisionHandle::LINE_WIDTH = 1.0;
const qreal Vector::PrecisionHandle::RATIO = 0.1;


Vector::Vector(qreal ax, qreal ay,
               qreal bx, qreal by,
               QColor color,
               QGraphicsItem * parent)
	: Shape(color,NULL)
	, QGraphicsPathItem(parent) {

	d_start =
		new PrecisionHandle([this]() {
			                    rebuild();
		                    },
			[this]() {
				rebuild();
				emit updated();
			});


	d_end =
		new PrecisionHandle([this]() {
			                    rebuild();
		                    },
			[this]() {
				rebuild();
				emit updated();
			});

	d_start->setPos(ax,ay);
	d_end->setPos(bx,by);
	d_start->setColor(d_color);
	d_end->setColor(d_color);

	setFlags(QGraphicsItem::ItemIsSelectable);
	rebuild();
}

Vector::~Vector() {
}

void Vector::addToScene(QGraphicsScene * scene) {
	scene->addItem(this);
	scene->addItem(d_start);
	scene->addItem(d_end);
}

void Vector::removeFromScene(QGraphicsScene * scene) {
	scene->removeItem(d_end);
	scene->removeItem(d_start);
	scene->removeItem(this);
	delete d_end;
	delete d_start;
}


QPointF Vector::startPos() const {
	return d_start->pos();
}

QPointF Vector::endPos() const {
	return d_end->pos();
}

void Vector::setEndPos(const QPointF & pos) {
	d_end->setPos(pos);
	rebuild();
}


void Vector::paint(QPainter * painter,
                   const QStyleOptionGraphicsItem * option,
                   QWidget * widget) {
	QColor actual = d_color;
	if ( isSelected() ) {
		actual = d_color.lighter(150);
	}
	actual.setAlpha(Shape::BORDER_OPACITY);
	setPen(QPen(actual,LINE_WIDTH));
	d_start->setColor(actual);
	d_end->setColor(actual);
	QStyleOptionGraphicsItem myOptions(*option);
	myOptions.state &= ~QStyle::State_Selected;
	QGraphicsPathItem::paint(painter,&myOptions,widget);

}

Vector::PrecisionHandle::PrecisionHandle(MovedCallback onMove,
                                         ReleasedCallback onRelease,
                                         QGraphicsItem * parent)
	: QGraphicsItemGroup(parent)
	, d_onMove(onMove)
	, d_onRelease(onRelease) {

	d_circle = new QGraphicsEllipseItem(-SIZE,-SIZE,2*SIZE,2*SIZE,this);
#define SQRT_2_2 0.70710678118
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
#undef SQRT_2_2

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
		setPath(QPainterPath());
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

	setPath(path);
}

void Vector::mousePressEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsPathItem::mousePressEvent(e);
	if ( d_moveEvent ) {
		return;
	}

	e->accept();
	d_moveEvent = std::make_shared<QPointF>(e->scenePos());
}

void Vector::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	if ( !d_moveEvent ) {
		e->ignore();
		return;
	}
	e->accept();
	moveUpdate(e->scenePos());
}

void Vector::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	if ( !d_moveEvent ) {
		e->ignore();
		return;
	}
	e->accept();
	moveUpdate(e->scenePos());
	d_moveEvent.reset();
	emit updated();
}


void Vector::moveUpdate(const QPointF & newPos) {
	auto delta = newPos - *d_moveEvent;
	d_start->setPos(d_start->pos() + delta);
	d_end->setPos(d_end->pos() + delta);
	*d_moveEvent = newPos;
	rebuild();
}
