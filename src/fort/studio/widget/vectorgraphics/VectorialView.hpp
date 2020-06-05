#pragma once


#include <QGraphicsView>

class VectorialView : public QGraphicsView {
	Q_OBJECT
public:
	explicit VectorialView(QWidget * parent);
	virtual ~VectorialView();

public slots:
	void resetZoom();
	void showEntireScene();
	void setBannerMessage(const QString & name,const QColor & color);
signals:
	void zoomed(double zoomFactor);

protected:
	void drawForeground(QPainter *painter, const QRectF &rect) override;
private:
	void zoom(double factor);

	bool eventFilter(QObject * object, QEvent * event) override;
	QPointF d_targetViewportPos,d_targetScenePos;
	double d_zoomFactorBase;
	QString d_bannerMessage;
	QColor  d_bannerColor;
};
