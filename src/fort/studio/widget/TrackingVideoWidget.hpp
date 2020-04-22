#pragma once

#include <QWidget>

#include "TrackingVideoFrame.hpp"

class IdentifierBridge;

class TrackingVideoWidget : public QWidget {
	Q_OBJECT
	Q_PROPERTY(bool showID
	           READ showID
	           WRITE setShowID
	           NOTIFY showIDChanged);
	Q_PROPERTY(bool showInteractions
	           READ showInteractions
	           WRITE setShowInteractions
	           NOTIFY showInteractionsChanged);

public:
	explicit TrackingVideoWidget(QWidget * parent = nullptr);
	~TrackingVideoWidget();


	void setup(IdentifierBridge *identifier);

	bool showID() const;

	bool showInteractions() const;


	fm::Time trackingTime() const;

	bool hasTrackingTime() const;

signals:
	void showIDChanged(bool value);
	void showInteractionsChanged(bool value);

	void hasTrackingTimeChanged(bool value);

public slots:
	void display(TrackingVideoFrame image);

	void hideLoadingBanner(bool hide);

	void setZoomFocus(quint32 antID,qreal value);

	void setShowID(bool show);
	void setShowInteractions(bool show);

protected:
	void paintEvent(QPaintEvent * event) override;

	void paintIdentifiedAnt(QPainter * painter, const QRectF & focusRectangle);

private:
	void focusAnt(quint32 antID, bool reset = false);

	void setHasTrackingTime(bool value);

	TrackingVideoFrame d_frame;
	IdentifierBridge * d_identifier;
	bool               d_hideLoadingBanner;
	bool               d_showID;
	bool               d_showInteractions;
	bool               d_hasTrackingTime;
	quint32            d_focusedAntID;
	qreal              d_zoom;
	QPointF            d_lastFocus;
};
