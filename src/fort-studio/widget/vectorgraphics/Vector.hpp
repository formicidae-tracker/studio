#pragma once

#include <QGraphicsItemGroup>

class Vector : public QGraphicsItemGroup {
public:
	const static qreal LINE_WIDTH;
	const static int   OPACITY;
	const static double ARROW_LENGTH;
	const static double ARROW_WIDTH;

	typedef std::function<void(const QPointF & ,const QPointF &)> UpdatedCallback;

	Vector(qreal ax, qreal ay,
	       qreal bx, qreal by,
	       QColor color,
	       UpdatedCallback onUpdated,
	       QGraphicsItem * parent = nullptr);
	virtual ~Vector();

	void setColor(const QColor & color);

	inline QGraphicsItem * startPrecisionHandle() const {
		return d_start;
	}

	inline QGraphicsItem * endPrecisionHandle() const {
		return d_end;
	}

protected:
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

private:
	class PrecisionHandle : public QGraphicsItemGroup {
	public:
		const static qreal SIZE;
		const static qreal LINE_WIDTH;
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
	void editFinished();

	UpdatedCallback     d_onUpdated;
	PrecisionHandle   * d_start, * d_end;
	QGraphicsPathItem * d_line;
	QColor              d_color;
};
