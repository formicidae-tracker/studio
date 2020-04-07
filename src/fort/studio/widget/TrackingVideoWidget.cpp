#include "TrackingVideoWidget.hpp"
#include "TrackingVideoFrame.hpp"


#include <fort/studio/Format.hpp>
#include <QPainter>

TrackingVideoWidget::TrackingVideoWidget(QWidget * parent)
	: QWidget(parent)
	, d_image(0,0) {
}

TrackingVideoWidget::~TrackingVideoWidget() {
}


void TrackingVideoWidget::display(QImage image) {
	d_image = image;
	update();
}

void TrackingVideoWidget::paintEvent(QPaintEvent * event) {
	QPainter painter(this);
	painter.fillRect(rect(),QColor(0,0,0));
	if ( d_image.isNull() == true ) {
		return;
	}
	auto size = d_image.size();
	size.scale(width(),height(),Qt::KeepAspectRatio);
	QRect targetRect(QPoint(0,0),size);
	targetRect.translate(rect().center()-targetRect.center());

	painter.drawImage(targetRect,d_image);
}
