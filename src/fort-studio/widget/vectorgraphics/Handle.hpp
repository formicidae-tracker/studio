#pragma once

#include <QGraphicsItemGroup>
#include <functional>

class Handle : public QGraphicsItemGroup {
public:
	const static int    SIZE;
	const static QColor COLOR;
	const static QColor SELECTED_COLOR;

	typedef std::function<void ()> MovedCallback;
	typedef std::function<void ()> ReleasedCallback;
	Handle(MovedCallback onMove,
	       ReleasedCallback onRelease,
	       QGraphicsItem * parent = nullptr);
	virtual ~Handle();

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent * e) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * e) override;
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;
private:
	QGraphicsRectItem * d_inside;
	MovedCallback       d_onMove;
	ReleasedCallback    d_onRelease;
};
