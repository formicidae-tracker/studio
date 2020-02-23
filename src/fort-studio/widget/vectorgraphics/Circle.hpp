#pragma once


#include <QGraphicsEllipseItem>
#include "Shape.hpp"

class Handle;

class Circle : public Shape, public QGraphicsEllipseItem {
	Q_OBJECT
public:

	Circle(const QPointF & pos,
	       qreal radius,
	       QColor color,
	       QGraphicsItem * parent = nullptr);
	virtual ~Circle();


	void addToScene(QGraphicsScene * scene) override;

	QPointF pos() const;

	qreal radius() const;



protected:
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

private:
	void update(bool fixRadius);

	Handle          * d_center, * d_radiusHandle;
	qreal             d_radius;
};
