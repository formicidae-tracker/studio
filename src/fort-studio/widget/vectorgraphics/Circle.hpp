#pragma once


#include <QGraphicsEllipseItem>


class Handle;

class Circle : public QGraphicsEllipseItem {
public:
	const static qreal LINE_WIDTH;
	const static int   BORDER_OPACITY;
	const static int   FILL_OPACITY;

	typedef std::function<void (const QPointF &, qreal)> UpdatedCallback;
	Circle(const QPointF & pos,
	       qreal radius,
	       QColor color,
	       UpdatedCallback onUpdated,
	       QGraphicsItem * parent = nullptr);
	virtual ~Circle();


	void addToScene(QGraphicsScene * scene);

	void setColor(const QColor & color);

protected:
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

private:
	void update(bool fixRadius);

	Handle          * d_center, * d_radiusHandle;
	qreal             d_radius;
	UpdatedCallback   d_onUpdated;
	QColor            d_color;
};
