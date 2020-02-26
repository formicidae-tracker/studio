#pragma once

#include <QGraphicsItemGroup>
#include <functional>
#include <QObject>

class Handle : public QObject, public QGraphicsItemGroup {
	Q_OBJECT
public:
	const static qreal    SIZE;
	const static QColor COLOR;
	const static QColor SELECTED_COLOR;

	typedef std::function<void ()> MovedCallback;
	typedef std::function<void ()> ReleasedCallback;

	Handle(MovedCallback onMove,
	       ReleasedCallback onRelease,
	       QGraphicsItem * parent = nullptr);
	virtual ~Handle();


	void addToScene(QGraphicsScene * scene);
public slots:
	void setScaleFactor(double factor);

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent * e) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * e) override;
private:
	QRectF build();

	QGraphicsRectItem * d_inside;
	MovedCallback       d_onMove;
	ReleasedCallback    d_onRelease;
	double              d_factor;
};
