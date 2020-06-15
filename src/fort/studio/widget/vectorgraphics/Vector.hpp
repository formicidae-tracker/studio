#pragma once

#include <QGraphicsPathItem>
#include "Shape.hpp"

#include <memory>


class Vector : public Shape, public QGraphicsPathItem {
	Q_OBJECT
public:
	const static double ARROW_LENGTH;
	const static double ARROW_WIDTH;


	Vector(qreal ax, qreal ay,
	       qreal bx, qreal by,
	       QColor color,
	       QGraphicsItem * parent = nullptr);
	virtual ~Vector();




	QPointF startPos() const;

	QPointF endPos() const;

	void setStartPos(const QPointF & pos);
	void setEndPos(const QPointF & pos);

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
	void moveUpdate(const QPointF & newPos);

	PrecisionHandle   * d_start, * d_end;
	std::shared_ptr<QPointF> d_moveEvent;
};
