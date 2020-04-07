#pragma once

#include <QWidget>

#include "TrackingVideoFrame.hpp"


class TrackingVideoWidget : public QWidget {
	Q_OBJECT
public:
	explicit TrackingVideoWidget(QWidget * parent = nullptr);
	~TrackingVideoWidget();


public slots:
	void display(TrackingVideoFrame image);

protected:
	void paintEvent(QPaintEvent * event) override;

	void paintIdentifiedAnt(QPainter * painter,
	                        const fmp::IdentifiedFrame::ConstPtr & frame);

private:
	QImage d_image;
};
