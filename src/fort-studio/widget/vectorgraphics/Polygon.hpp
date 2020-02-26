#pragma once


#include <QGraphicsPolygonItem>
#include "Shape.hpp"

class Handle;

class Polygon : public Shape, public QGraphicsPolygonItem {
public:
	Polygon(const QVector<QPointF> & points,
	        QColor color,
	        QGraphicsItem * parent = nullptr);

	virtual ~Polygon();

	void addToScene(QGraphicsScene * scene);

	QVector<QPointF> vertices() const;

	Handle * appendPoint(const QPointF & point);

	void close();

protected:
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

private:
	void update(size_t i);

private:
	QVector<Handle*> d_handles;
};
