#pragma once

#include <QGraphicsItemGroup>
#include "Shape.hpp"
class Vector : public Shape, public QGraphicsItemGroup {
	Q_OBJECT
public:
	const static double ARROW_LENGTH;
	const static double ARROW_WIDTH;


	Vector(qreal ax, qreal ay,
	       qreal bx, qreal by,
	       QColor color,
	       QGraphicsItem * parent = nullptr);
	virtual ~Vector();


	void addToScene(QGraphicsScene * scene) override;


	QPointF startPos() const;

	QPointF endPos() const;

	void setEndPos(const QPointF & pos);

protected:
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

private:
	class PrecisionHandle : public QGraphicsItemGroup {
	public:
		const static qreal SIZE;
		const static qreal LINE_WIDTH;
		const static qreal RATIO;
		typedef std::function<void ()> MovedCallback;
		typedef std::function<void ()> ReleasedCallback;

		PrecisionHandle(MovedCallback onMove,
		                ReleasedCallback onRelease,
		                QGraphicsItem * parent = nullptr);
		virtual ~PrecisionHandle();

		void setColor(const QColor & color);
	protected:
		void mouseMoveEvent(QGraphicsSceneMouseEvent * e) override;
		void mouseReleaseEvent(QGraphicsSceneMouseEvent * e) override;
	private:
		QGraphicsEllipseItem * d_circle;
		QGraphicsLineItem    * d_lines[4];
		MovedCallback          d_onMove;
		ReleasedCallback       d_onRelease;

	};

	void rebuild();

	PrecisionHandle   * d_start, * d_end;
	QGraphicsPathItem * d_line;
};
