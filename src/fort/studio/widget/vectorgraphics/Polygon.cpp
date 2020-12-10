#include "Polygon.hpp"

#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "Handle.hpp"

#include <fort/studio/Utils.hpp>


Polygon::Polygon(const QVector<QPointF> & points,
                 QColor color,
                 QGraphicsItem * parent)
	: Shape(color,NULL)
	, QGraphicsPolygonItem(QPolygonF(points),parent) {

	auto p = polygon();
	size_t i = 0;
	size_t size = p.size();
	if ( p.size() > 2 && p.isClosed() ) {
		size -= 1;
	}
	for ( size_t i = 0; i < size; ++i ) {
		auto h = new Handle([this,i]() {
			                    update(i);
		                    },
			[this,i]() {
				update(i);
				emit updated();
			});
		h->setPos(p[i]);
		d_handles.push_back(h);
	}

	setFlag(QGraphicsItem::ItemIsSelectable,true);
}

Polygon::~Polygon() {
}

void Polygon::addToSceneProtected(QGraphicsScene * scene) {
	scene->addItem(this);
	for ( const auto & h : d_handles ){
		h->addToScene(scene);
	}
}


void Polygon::removeFromSceneProtected(QGraphicsScene * scene) {
	for ( const auto & h : d_handles ) {
		scene->removeItem(h);
		h->deleteLater();
	}
	scene->removeItem(this);
}

Handle * Polygon::appendPoint(const QPointF & point) {
	auto p = polygon();
	if ( p.size() > 1 && p.isClosed() == true ) {
		return nullptr;
	}

	auto last = ToEigen(p.back());
	auto newPoint = ToEigen(point);
	if ((newPoint-last).norm() <= Handle::SIZE ) {
		if ( p.size() < 3 ) {
			return nullptr;
		}
	}

	size_t i = p.size();
	p.push_back(point);
	auto h = new Handle([this,i]() {
		                    update(i);
	                    },
		[this,i]() {
			update(i);
			emit updated();
		});
	h->setPos(point);
	setPolygon(p);
	d_handles.push_back(h);
	return h;
}

void Polygon::updateLast(const QPointF & point) {
	auto p = polygon();
	p.last() = point;
	d_handles.back()->setPos(point);
	setPolygon(p);
}

void Polygon::removeLast() {
	delete d_handles.back();
	d_handles.pop_back();
	auto p = polygon();
	p.pop_back();
	setPolygon(p);
}

void Polygon::close() {
	auto p = polygon();
	if ( p.isClosed() == true || p.size() < 2 ) {
		return;
	}
	p.push_back(p.first());
	setPolygon(p);
}


QVector<QPointF> Polygon::vertices() const {
	QVector<QPointF> vertices;
	for ( const auto & h : d_handles ) {
		vertices.push_back(h->pos());
	}
	return vertices;
}


void Polygon::paint(QPainter * painter,
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
	QStyleOptionGraphicsItem myOptions(*option);
	myOptions.state &= ~QStyle::State_Selected;
	QGraphicsPolygonItem::paint(painter,&myOptions,widget);
}


void Polygon::update(size_t i) {
	auto p = polygon();

	size_t number = p.size();
	if ( p.isClosed() == true ) {
		number -= 1;
	}

	if ( p.size() < 2 || i >= number ) {
		return;
	}
	p[i] = d_handles[i]->pos();
	if ( i == 0 ) {
		p[number] = d_handles[i]->pos();
	}
	setPolygon(p);
}



void Polygon::mousePressEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsPolygonItem::mousePressEvent(e);
	if ( d_moveEvent  ) {
		return;
	}
	d_moveEvent = std::make_shared<QPointF>(e->scenePos());
	e->accept();
}
void Polygon::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	if ( !d_moveEvent ) {
		e->ignore();
		return;
	}
	moveUpdate(e->scenePos());
	e->accept();
}

void Polygon::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	if ( !d_moveEvent ) {
		e->ignore();
		return;
	}
	moveUpdate(e->scenePos());
	e->accept();
	emit updated();
	d_moveEvent.reset();
}

void Polygon::moveUpdate(const QPointF & newPos) {
	if ( !d_moveEvent ) {
		return;
	}

	QPointF delta = newPos - *d_moveEvent;
	for ( const auto & h : d_handles ) {
		h->setPos(h->pos() + delta);
	}
	auto p = polygon();
	for ( auto & vertex : p ) {
		vertex += delta;
	}
	setPolygon(p);
	*d_moveEvent = newPos;
}
