#pragma once

#include <QObject>
#include <QColor>

class QGraphicsScene;

class Shape : public QObject {
	Q_OBJECT
public :
	const static qreal LINE_WIDTH;
	const static int   BORDER_OPACITY;
	const static int   FILL_OPACITY;

	Shape(const QColor & color, QObject * parent);
	virtual ~Shape();

	void setColor(const QColor & color);

	void addToScene( QGraphicsScene * scene );
	void removeFromScene( QGraphicsScene * scene );
protected:
	virtual void addToSceneProtected( QGraphicsScene * scene ) = 0;
	virtual void removeFromSceneProtected( QGraphicsScene * scene ) = 0;

signals:
	void colorUpdated();
	void updated();

protected:
	QColor d_color;
};
