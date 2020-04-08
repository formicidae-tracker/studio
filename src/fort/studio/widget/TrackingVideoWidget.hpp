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

public slots:
	void display(TrackingVideoFrame image);

	void hideLoadingBanner(bool hide);

protected:
	void paintEvent(QPaintEvent * event) override;

	void paintIdentifiedAnt(QPainter * painter,
	                        const fmp::IdentifiedFrame::ConstPtr & frame,
	                        int targetHeigth);

private:
	QImage             d_image;
	IdentifierBridge * d_identifier;
	bool               d_hideLoadingBanner;
};
