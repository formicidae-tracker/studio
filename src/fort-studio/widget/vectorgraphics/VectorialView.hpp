#pragma once


#include <QGraphicsView>

class VectorialView : public QGraphicsView {
	Q_OBJECT
public:
	explicit VectorialView(QWidget * parent);
	virtual ~VectorialView();

public slots:
	void resetZoom();

signals:
	void zoomed(double zoomFactor);
private:
	void zoom(double factor);

	bool eventFilter(QObject * object, QEvent * event);
	QPointF d_targetViewportPos,d_targetScenePos;
	double d_zoomFactorBase;
};
