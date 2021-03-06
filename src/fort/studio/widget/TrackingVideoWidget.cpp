#include "TrackingVideoWidget.hpp"
#include "TrackingVideoFrame.hpp"


#include <fort/studio/Format.hpp>

#include <fort/studio/bridge/AntDisplayBridge.hpp>
#include <fort/studio/MyrmidonTypes/Conversion.hpp>

#include <QPainter>
#include <QMouseEvent>

#include <QDebug>


TrackingVideoWidget::TrackingVideoWidget(QWidget * parent)
	: QWidget(parent)
	, d_antDisplay(nullptr)
	, d_hideLoadingBanner(true)
	, d_showID(false)
	, d_showCollisions(false)
	, d_focusedAntID(0)
	, d_zoom(1.0)
	, d_lastFocus(0,0)
	, d_hasTrackingTime(false) {
}

TrackingVideoWidget::~TrackingVideoWidget() {
}

bool TrackingVideoWidget::showID() const {
	return d_showID;
}

bool TrackingVideoWidget::showCollisions() const {
	return d_showCollisions;
}


void TrackingVideoWidget::display(TrackingVideoFrame frame) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[widget] Received frame:" << frame << std::endl);
	d_frame = frame;
	setHasTrackingTime(!d_frame.TrackingFrame == false);

	update();
}

void TrackingVideoWidget::paintEvent(QPaintEvent * event) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[widget] Paint Event with frame:" << d_frame << std::endl);

	QPainter painter(this);
	painter.setRenderHint(QPainter::SmoothPixmapTransform,true);
	painter.fillRect(rect(),QColor(0,0,0));
	if ( d_frame.Image == nullptr ) {
		return;
	}

	QImage image = *d_frame.Image;

	if ( d_focusedAntID != 0 ) {
		focusAnt(d_focusedAntID);
	}

	QRectF sourceRect(0,0,image.width(),image.height());
	if ( d_zoom > 1.0 ) {
		sourceRect.setSize(sourceRect.size()/d_zoom);
		QPointF actualFocus = d_lastFocus;
		if ( actualFocus.x() - sourceRect.width()/2.0 < 0 ) {
			actualFocus.rx() = sourceRect.width()/2.0;
		}
		if (actualFocus.x() + sourceRect.width()/2.0 > image.width() ) {
			actualFocus.rx() = image.width() - sourceRect.width() / 2.0;
		}
		if ( actualFocus.y() - sourceRect.height()/2.0 < 0 ) {
			actualFocus.ry() = sourceRect.height()/2.0;
		}
		if (actualFocus.y() + sourceRect.height()/2.0 > image.height() ) {
			actualFocus.ry() = image.height() - sourceRect.height() / 2.0;
		}

		sourceRect.translate(actualFocus - sourceRect.center());

	}

	if ( !d_frame.TrackingFrame == false && d_antDisplay != nullptr ) {
		QPainter imagePainter(&image);
		imagePainter.setRenderHint(QPainter::Antialiasing,true);
		imagePainter.setRenderHint(QPainter::TextAntialiasing,true);
		paintIdentifiedAnt(&imagePainter,sourceRect);
	}


	auto size = image.size();
	size.scale(width(),height(),Qt::KeepAspectRatio);
	QRect targetRect(QPoint(0,0),size);
	targetRect.translate(rect().center()-targetRect.center());

	painter.drawImage(targetRect,image,sourceRect);

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

void TrackingVideoWidget::setup(AntDisplayBridge * antDisplay) {
	d_antDisplay = antDisplay;

	connect(d_antDisplay,
	        &AntDisplayBridge::antDisplayChanged,
	        this,
	        static_cast<void (QWidget::*)()>(&QWidget::update));

}



void TrackingVideoWidget::paintIdentifiedAnt(QPainter * painter, const QRectF & focusRectangle) {
	VIDEO_PLAYER_DEBUG(std::cerr << "[widget] identification painting on:" << d_frame << std::endl);
	const auto & tFrame = d_frame.TrackingFrame;
	const auto & iFrame = d_frame.CollisionFrame;
	double ratio = double(d_frame.Image->height()) / double(tFrame->Height);
	const static double ANT_HALF_SIZE = 8.0;

	painter->setPen(Qt::NoPen);

	auto font = painter->font();
	font.setPointSizeF(font.pointSizeF() * double(d_frame.Image->height()) / double(height()) * 0.8);
	painter->setFont(font);
	auto metrics = QFontMetrics(font);
	bool hasSolo = d_antDisplay->numberSoloAnt() != 0;

	if ( !iFrame == false && d_showCollisions == true ) {
		fmp::DenseMap<quint32,fm::PositionedAnt> positions;
		for ( const auto & pa : tFrame->Positions ) {
			positions.insert(std::make_pair(pa.ID,pa));
		}

		for ( const auto & collision : iFrame->Collisions ) {
			auto aID = collision.IDs.first;
			auto bID = collision.IDs.second;
			const auto & [aDisplayStatus,aDisplayColor] = d_antDisplay->displayStatusAndColor(aID);
			const auto & [bDisplayStatus,bDisplayColor] = d_antDisplay->displayStatusAndColor(bID);

			if ( aDisplayStatus != fmp::Ant::DisplayState::HIDDEN
			     || bDisplayStatus != fmp::Ant::DisplayState::HIDDEN
			     || ( hasSolo == true
			          && aDisplayStatus != fmp::Ant::DisplayState::SOLO
			          && bDisplayStatus != fmp::Ant::DisplayState::SOLO) ) {
				continue;
			}

			auto aPos = Conversion::fromEigen(ratio * positions.at(aID).Position);
			auto bPos = Conversion::fromEigen(ratio * positions.at(bID).Position);

			if ( focusRectangle.contains(aPos) == false
			     && focusRectangle.contains(bPos) == false ) {
				continue;
			}
			auto c = Conversion::colorFromFM(aDisplayColor,150);
			painter->setPen(QPen(c,3));

			painter->drawLine(aPos,bPos);

		}
	}


	for ( const auto & pa : tFrame->Positions ) {
		const auto & [displayStatus,displayColor] = d_antDisplay->displayStatusAndColor(pa.ID);
		if ( displayStatus == fmp::Ant::DisplayState::HIDDEN
		     || ( hasSolo == true && displayStatus != fmp::Ant::DisplayState::SOLO) ) {
			continue;
		}
		auto correctedPos = Conversion::fromEigen(ratio * pa.Position);

		if ( focusRectangle.contains(correctedPos) == false ) {
			continue;
		}

		auto c = Conversion::colorFromFM(displayColor,150);
		painter->setPen(Qt::NoPen);
		painter->setBrush(c);


		painter->translate(correctedPos);
		painter->rotate(pa.Angle * 180.0 / M_PI);

		painter->drawEllipse(QRectF(-QPointF(ANT_HALF_SIZE,ANT_HALF_SIZE),
		                            QSize(ANT_HALF_SIZE * 2.0,
		                                  ANT_HALF_SIZE * 2.0)));

		painter->drawEllipse(QRectF(QPointF(ANT_HALF_SIZE,-ANT_HALF_SIZE*0.6),
		                            QSize(ANT_HALF_SIZE*1.2,
		                                  ANT_HALF_SIZE*1.2)));

		painter->resetTransform();


		if ( d_showID == true ) {

			QString idStr = fmp::Ant::FormatID(pa.ID).c_str();
			auto bRect = metrics.boundingRect(idStr);

			bRect.translate((correctedPos + QPointF(-bRect.width()/2,bRect.height() + ANT_HALF_SIZE)).toPoint());

			painter->setBrush(QColor(255,255,255,100));
			painter->drawRect(bRect);

			painter->setPen(Qt::black);
			painter->drawText(bRect,Qt::AlignCenter,idStr);
		}
	}

}

void TrackingVideoWidget::hideLoadingBanner(bool hide) {
	if ( hide == d_hideLoadingBanner ) {
		return;
	}
	d_hideLoadingBanner = hide;
	update();
}


void TrackingVideoWidget::setZoomFocus(quint32 antID,qreal value) {
	if ( d_zoom == value && antID == d_focusedAntID ) {
		return;
	}

	if ( antID != d_focusedAntID ) {
		focusAnt(antID,true);
	}

	d_focusedAntID = antID;
	d_zoom = std::max(1.0,value);

	update();
}


void TrackingVideoWidget::focusAnt(quint32 antID, bool reset) {
	if ( !d_frame.Image == true || !d_frame.TrackingFrame ) {
		if ( reset == true ) {
			d_lastFocus = QPointF(0,0);
		}
		return;
	}

	for ( const auto & ap : d_frame.TrackingFrame->Positions ) {
		if ( ap.ID == antID ) {
			double ratio = double(d_frame.Image->height())/double(d_frame.TrackingFrame->Height);
			d_lastFocus = QPointF(ratio * ap.Position.x(),ratio * ap.Position.y());
			return;
		}
	}
	if ( reset == true ) {
		d_lastFocus = QPointF(0,0);
	}
}


void TrackingVideoWidget::setShowID(bool show) {
	if ( show == d_showID ) {
		return;
	}
	d_showID = show;
	update();
	emit showIDChanged(show);
}

void TrackingVideoWidget::setShowCollisions(bool show) {
	if ( show == d_showCollisions ) {
		return;
	}
	d_showCollisions = show;
	update();
	emit showCollisionsChanged(show);
}


void TrackingVideoWidget::setHasTrackingTime(bool value) {
	if ( value == d_hasTrackingTime ) {
		return;
	}
	d_hasTrackingTime = value;
	emit hasTrackingTimeChanged(value);
}

bool TrackingVideoWidget::hasTrackingTime() const {
	return d_hasTrackingTime;
}

fm::Time TrackingVideoWidget::trackingTime() const {
	if ( !d_frame.TrackingFrame == true ) {
		return fm::Time();
	}
	return d_frame.TrackingFrame->FrameTime;
}


void TrackingVideoWidget::mousePressEvent(QMouseEvent * event) {
	if ( event->button() == Qt::LeftButton ) {
		emit togglePlayPause();
	}
	event->accept();
}

void TrackingVideoWidget::mouseDoubleClickEvent(QMouseEvent * event) {
	if ( event->button() == Qt::LeftButton ) {
		// qInfo() << "Fullscreen is not implemented";

		// //we already received a click so we toggle again
		emit togglePlayPause();
	}
	event->accept();
}
