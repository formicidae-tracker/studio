#include "Polygon.hpp"

#include <QGraphicsScene>

#include "Handle.hpp"

#include <iostream>

const qreal Polygon::LINE_WIDTH = 1.5;
const int   Polygon::BORDER_OPACITY = 200;
const int   Polygon::FILL_OPACITY = 40;


Polygon::Polygon(const QVector<QPointF> & points,
                 QColor color,
                 UpdatedCallback onUpdated,
                 QGraphicsItem * parent)
	: QGraphicsPolygonItem(QPolygonF(points),parent)
	, d_onUpdated(onUpdated)
	, d_color(color) {

	auto p = polygon();
	size_t i = 0;
	size_t size = p.size();
	if ( p.size() > 2 && p.isClosed() ) {
		std::cerr << "Closed" << std::endl;
		size -= 1;
	}
	for ( size_t i = 0; i < size; ++i ) {
		auto h = new Handle([this,i]() {
			                    update(i);
		                    },
			[this,i]() {
				update(i);
				d_onUpdated(polygon());
			});
		h->setPos(p[i]);
		d_handles.push_back(h);
	}

	setFlags(QGraphicsItem::ItemIsSelectable);
}

Polygon::~Polygon() {
}

void Polygon::addToScene(QGraphicsScene * scene) {
	scene->addItem(this);
	for ( const auto & h : d_handles ){
		scene->addItem(h);
	}
}


QGraphicsItem * Polygon::appendPoint(const QPointF & point) {
	auto p = polygon();
	if ( p.isClosed() == true ) {
		return nullptr;
	}

	size_t i = p.size();
	p.push_back(point);
	auto h = new Handle([this,i]() {
		                    update(i);
	                    },
		[this,i]() {
			update(i);
			d_onUpdated(polygon());
		});
	setPolygon(p);
	d_handles.push_back(h);
}

void Polygon::close() {
	auto p = polygon();
	if ( p.isClosed() == true || p.size() < 2 ) {
		return;
	}
	p.push_back(p.first());
	setPolygon(p);
}

void Polygon::setColor(const QColor & color) {
	d_color = color;
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
	QGraphicsPolygonItem::paint(painter,option,widget);
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
