#include "TrackingVideoWidget.hpp"
#include "TrackingVideoFrame.hpp"


#include <fort/studio/Format.hpp>

#include <fort/studio/bridge/IdentifierBridge.hpp>


#include <QPainter>



TrackingVideoWidget::TrackingVideoWidget(QWidget * parent)
	: QWidget(parent)
	, d_identifier(nullptr)
	, d_hideLoadingBanner(true) {
}

TrackingVideoWidget::~TrackingVideoWidget() {
}

void TrackingVideoWidget::display(TrackingVideoFrame frame) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[widget] Received frame:" << frame << std::endl);
	d_frame = frame;
	update();
}

void TrackingVideoWidget::paintEvent(QPaintEvent * event) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[widget] Paint Event with frame:" << d_frame << std::endl);

	QPainter painter(this);
	painter.fillRect(rect(),QColor(0,0,0));
	if ( !d_frame.Image == true ) {
		return;
	}

	QImage image = *d_frame.Image;

	if ( !d_frame.TrackingFrame == false && d_identifier != nullptr ) {
		QPainter imagePainter(&image);
		imagePainter.setRenderHint(QPainter::Antialiasing,true);
		paintIdentifiedAnt(&imagePainter);
	}

	auto size = image.size();
	size.scale(width(),height(),Qt::KeepAspectRatio);
	QRect targetRect(QPoint(0,0),size);
	targetRect.translate(rect().center()-targetRect.center());

	painter.drawImage(targetRect,image);

	if ( d_hideLoadingBanner == false ) {
		auto font = painter.font();
		font.setPointSize(14);
		painter.setFont(font);
		painter.setBrush(QColor(255,255,255,150));
		painter.setPen(Qt::NoPen);
		auto rect = QRectF(0,0,width(),60);

		painter.drawRect(rect);

		painter.setPen(QColor(0,0,0));
		painter.drawText(rect,
		                 Qt::AlignCenter,
		                 tr("Tracking Data Loading (available displayed, but seek disabled)"));
	}
}

void TrackingVideoWidget::setup(IdentifierBridge *identifier) {
	d_identifier = identifier;

	connect(d_identifier,
	        &IdentifierBridge::antDisplayChanged,
	        this,
	        static_cast<void (QWidget::*)()>(&QWidget::update));

}



void TrackingVideoWidget::paintIdentifiedAnt(QPainter * painter) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[widget] identification painting on:" << d_frame << std::endl);
	const auto & tFrame = d_frame.TrackingFrame;
	double ratio = double(d_frame.Image->height()) / double(tFrame->Height);
	const static double ANT_HALF_SIZE = 10.0;

	painter->setPen(Qt::NoPen);

	bool hasSolo = d_identifier->numberSoloAnt() != 0;
	for ( const auto & pa : tFrame->Positions ) {
		auto a = d_identifier->ant(pa.ID);
		if ( !a
		     || ( hasSolo == true && a->DisplayStatus() != fmp::Ant::DisplayState::SOLO)
		     || a->DisplayStatus() == fmp::Ant::DisplayState::HIDDEN ) {
			continue;
		}
		auto c = Conversion::colorFromFM(a->DisplayColor(),150);
		painter->setBrush(c);
		painter->drawEllipse(QRectF(ratio * pa.Position.x() - ANT_HALF_SIZE,
		                            ratio * pa.Position.y() - ANT_HALF_SIZE,
		                            ANT_HALF_SIZE * 2.0,
		                            ANT_HALF_SIZE * 2.0));
	}

}

void TrackingVideoWidget::hideLoadingBanner(bool hide) {
	if ( hide == d_hideLoadingBanner ) {
		return;
	}
	d_hideLoadingBanner = hide;
	update();
}
