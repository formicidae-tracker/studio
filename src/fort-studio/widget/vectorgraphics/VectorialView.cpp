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
		if (QApplication::keyboardModifiers() == Qt::ShiftModifier
		    && wheelEvent->orientation() == Qt::Vertical ) {
			d_targetViewportPos = wheelEvent->pos();
			d_targetScenePos = mapToScene(wheelEvent->pos());
			double angle = wheelEvent->angleDelta().y();
			double factor = std::pow(d_zoomFactorBase, angle);
			zoom(factor);
			return true;
		}
	}
	return false;
}
