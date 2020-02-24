#pragma once


#include "Handle.hpp"

class Capsule : public QGraphicsItemGroup {
public:
	const static qreal LINE_WIDTH;
	const static int   BORDER_OPACITY;
	const static int   FILL_OPACITY;
	const static qreal MIN_DISTANCE;
	typedef std::function<void(const QPointF &,const QPointF &,qreal,qreal)> UpdatedCallback;

	Capsule(const QPointF & c1, const QPointF & c2,
	        qreal r1, qreal r2,
	        QColor color,
	        UpdatedCallback onUpdated,
	        QGraphicsItem * parent = nullptr);

	virtual ~Capsule();

	void setColor(const QColor & color);

	inline QGraphicsItem * c1Handle() const {
		return d_c1;
	}

	inline QGraphicsItem * c2Handle() const {
		return d_c2;
	}

	inline QGraphicsItem * r1Handle() const {
		return d_r1;
	}

	inline QGraphicsItem * r2Handle() const {
		return d_r2;
	}

protected:
	void paint(QPainter * painter,
	           const QStyleOptionGraphicsItem * option,
	           QWidget * widget) override;

private:
	void updateCenter(Handle * center);
	void updateRadius(Handle * radius);
	void editFinished() const;
	void rebuild();


	Handle            * d_c1,*d_c2,*d_r1,*d_r2;
	qreal               d_radius1,d_radius2;
	QColor              d_color;
	QGraphicsPathItem * d_path;
	UpdatedCallback     d_onUpdated;
};
