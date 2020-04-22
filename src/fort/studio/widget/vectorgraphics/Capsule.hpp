#pragma once


#include "Handle.hpp"
#include "Shape.hpp"

#include <QGraphicsPathItem>

#include <memory>

class Capsule : public Shape, public QGraphicsPathItem {
	Q_OBJECT
public:
	const static qreal MIN_DISTANCE;

	Capsule(const QPointF & c1, const QPointF & c2,
	        qreal r1, qreal r2,
	        QColor color,
	        QGraphicsItem * parent = nullptr);

	virtual ~Capsule();


	void setC2AndRadiusFromPos(const QPointF & pos);

	QPointF c1Pos() const;
	QPointF c2Pos() const;

	qreal r1() const;
	qreal r2() const;

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
	void updateCenter(Handle * center);
	void updateRadius(Handle * radius);
	void rebuild();
	void moveUpdate(const QPointF & newPos);

	Handle                 * d_c1,*d_c2,*d_r1,*d_r2;
	qreal                    d_radius1,d_radius2;
	std::shared_ptr<QPointF> d_moveEvent;
};
