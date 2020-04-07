#pragma once

#include <QWidget>
#include <QImage>



class TrackingVideoWidget : public QWidget {
	Q_OBJECT
public:
	explicit TrackingVideoWidget(QWidget * parent = nullptr);
	~TrackingVideoWidget();

public slots:
	void display(QImage image);

protected:
	void paintEvent(QPaintEvent * event) override;
private:
	QImage d_image;
};
