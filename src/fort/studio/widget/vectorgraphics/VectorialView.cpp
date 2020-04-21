#include "VectorialView.hpp"

#include <QDebug>
#include <QWheelEvent>
#include <QApplication>
#include <cmath>
#include <QMetaObject>

#include "VectorialScene.hpp"

VectorialView::VectorialView(QWidget * parent)
	: QGraphicsView(parent) {
	viewport()->installEventFilter(this);
	d_targetViewportPos = QPointF(1e8,1e8);
	d_zoomFactorBase = 1.0015;
}

void VectorialView::setBannerMessage(const QString & bannerMessage, const QColor & color) {
	if ( bannerMessage == d_bannerMessage && color == d_bannerColor) {
		return;
	}
	d_bannerMessage = bannerMessage;
	d_bannerColor = color;
	d_bannerColor.setAlpha(200);
	if ( d_bannerMessage.isEmpty() == true) {
		setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	} else {
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	}
	if ( scene() != nullptr ) {
		scene()->invalidate();
	}
}


VectorialView::~VectorialView() {
}

void VectorialView::resetZoom() {
	if ( transform().m11() == 1.0 ) {
		return;
	}
	setTransform(QTransform());
	centerOn(d_targetScenePos);
	emit zoomed(1.0);
}

void VectorialView::showEntireScene() {
	if ( scene() == nullptr ) {
		return;
	}
	auto mySize = viewport()->size();
	auto target = scene()->sceneRect();
	double factor = std::min(double(mySize.width())/target.width(),double(mySize.height())/target.height());
	resetZoom();
	zoom(factor);
}

void VectorialView::zoom(double factor) {
	scale(factor, factor);
	centerOn(d_targetScenePos);
	QPointF deltaViewportPos = d_targetViewportPos - QPointF(viewport()->width() / 2.0,
	                                                         viewport()->height() / 2.0);
	QPointF viewportCenter = mapFromScene(d_targetScenePos) - deltaViewportPos;
	centerOn(mapToScene(viewportCenter.toPoint()));
	emit zoomed(transform().m11());
}

bool VectorialView::eventFilter(QObject * object, QEvent * event) {
	if ( event->type() == QEvent::Wheel ) {
		QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
		if (QApplication::keyboardModifiers() == Qt::ShiftModifier ) {
			if ( wheelEvent->orientation() == Qt::Vertical ) {
				d_targetViewportPos = wheelEvent->pos();
				d_targetScenePos = mapToScene(wheelEvent->pos());
				double angle = wheelEvent->angleDelta().y();
				double factor = std::pow(d_zoomFactorBase, angle);
				zoom(factor);
			}
			// also disables horizontal scrolling, useful for laptop gestures
			return true;
		}
	}
	return false;
}

void VectorialView::drawForeground(QPainter *painter, const QRectF &rect) {
	if ( d_bannerMessage.isEmpty() ) {
		return;
	}
	QRectF bannerRect(rect.topLeft(),QSizeF(rect.width(),60));
	painter->fillRect(bannerRect,d_bannerColor);
	auto font = painter->font();
	font.setPointSize(14);
	painter->setFont(font);
	painter->drawText(bannerRect,Qt::AlignCenter,d_bannerMessage);
}
