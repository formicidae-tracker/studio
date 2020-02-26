#pragma once


#include "Handle.hpp"
#include "Shape.hpp"

class Capsule : public Shape, public QGraphicsItemGroup {
	Q_OBJECT
public:
	const static qreal MIN_DISTANCE;

	Capsule(const QPointF & c1, const QPointF & c2,
	        qreal r1, qreal r2,
	        QColor color,
	        QGraphicsItem * parent = nullptr);

	virtual ~Capsule();

	void addToScene(QGraphicsScene * scene) override;

	QPointF c1Pos() const;
	QPointF c2Pos() const;

	qreal r1() const;
	qreal r2() const;

protected:
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

private:
	void updateCenter(Handle * center);
	void updateRadius(Handle * radius);
	void rebuild();


	Handle            * d_c1,*d_c2,*d_r1,*d_r2;
	qreal               d_radius1,d_radius2;
	QGraphicsPathItem * d_path;
};
