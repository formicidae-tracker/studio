#pragma once

#include <QWidget>

#include "TrackingVideoFrame.hpp"

class IdentifierBridge;

class TrackingVideoWidget : public QWidget {
	Q_OBJECT
public:
	explicit TrackingVideoWidget(QWidget * parent = nullptr);
	~TrackingVideoWidget();


	void setup(IdentifierBridge *identifier);

	bool showID() const;

signals:
	void showIDChanged(bool value);

public slots:
	void display(TrackingVideoFrame image);

	void hideLoadingBanner(bool hide);

	void setZoomFocus(quint32 antID,qreal value);

	void setShowID(bool show);

protected:
	void paintEvent(QPaintEvent * event) override;

	void paintIdentifiedAnt(QPainter * painter, const QRectF & focusRectangle);

private:
	void focusAnt(quint32 antID, bool reset = false);


	TrackingVideoFrame d_frame;
	IdentifierBridge * d_identifier;
	bool               d_hideLoadingBanner;
	bool               d_showID;
	quint32            d_focusedAntID;
	qreal              d_zoom;
	QPointF            d_lastFocus;
};
