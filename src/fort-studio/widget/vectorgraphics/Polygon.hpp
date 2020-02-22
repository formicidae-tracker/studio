#pragma once


#include <QGraphicsPolygonItem>

class Handle;

class Polygon : public QGraphicsPolygonItem {
public:
	const static qreal LINE_WIDTH;
	const static int   BORDER_OPACITY;
	const static int   FILL_OPACITY;

	typedef std::function<void (const QVector<QPointF>&)> UpdatedCallback;
	Polygon(const QVector<QPointF> & points,
	        QColor color,
	        UpdatedCallback onUpdated,
	        QGraphicsItem * parent);

	virtual ~Polygon();

	void addToScene(QGraphicsScene * scene);


	void setColor(const QColor & color);

	QGraphicsItem * appendPoint(const QPointF & point);

	void close();

protected:
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

private:
	void update(size_t i);

private:
	QVector<Handle*> d_handles;
	UpdatedCallback  d_onUpdated;
	QColor           d_color;
};
