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


void TrackingVideoWidget::display(TrackingVideoFrame frame) {
	VIDEO_PLAYER_DEBUG(std::cerr << "Received frame:" << frame << std::endl);

	if ( !frame.Image ) {
		d_image = QImage(0,0);
		update();
		return;
	}
	d_image = *frame.Image;
	QPainter painter(&d_image);
	painter.setRenderHint(QPainter::Antialiasing,true);
	if ( !frame.TrackingFrame == false ) {
		paintIdentifiedAnt(&painter,frame.TrackingFrame);
	}
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


void TrackingVideoWidget::paintIdentifiedAnt(QPainter * painter,
                                             const fmp::IdentifiedFrame::ConstPtr & frame) {

}
