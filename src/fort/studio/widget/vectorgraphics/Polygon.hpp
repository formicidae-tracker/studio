#pragma once


#include <QGraphicsPolygonItem>
#include "Shape.hpp"

#include <memory>

class Handle;

class Polygon : public Shape, public QGraphicsPolygonItem {
	Q_OBJECT
public:
	Polygon(const QVector<QPointF> & points,
	        QColor color,
	        QGraphicsItem * parent = nullptr);

	virtual ~Polygon();


	QVector<QPointF> vertices() const;

	Handle * appendPoint(const QPointF & point);

	void close();

protected:
	void addToSceneProtected(QGraphicsScene * scene) override;
	void removeFromSceneProtected(QGraphicsScene * scene) override;

	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

	void mousePressEvent(QGraphicsSceneMouseEvent * e) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent * e) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * e) override;

private:
	void update(size_t i);
	void moveUpdate(const QPointF & newPos);
private:
	QVector<Handle*> d_handles;
	std::shared_ptr<QPointF> d_moveEvent;
};
