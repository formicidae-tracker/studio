#include "Handle.hpp"


#include <QPen>
#include "VectorialScene.hpp"

const qreal Handle::SIZE = 9;
const QColor Handle::COLOR = QColor(127,127,127,255);
const QColor Handle::SELECTED_COLOR = QColor(200,200,200,255);


QRectF Handle::build() {
	return QRectF(-d_factor * SIZE/2,
	              -d_factor * SIZE/2,
	              d_factor * SIZE,
	              d_factor * SIZE);
}


Handle::Handle(MovedCallback onMove,
               ReleasedCallback onRelease,
               QGraphicsItem * parent)
	: QGraphicsItemGroup(parent)
	, d_onMove(onMove)
	, d_onRelease(onRelease)
	, d_factor(1.0) {
	QPen empty;
	empty.setStyle(Qt::NoPen);
	d_inside = new QGraphicsRectItem(build(),this);
	d_inside->setPen(empty);
	d_inside->setBrush(COLOR);
	setFlags(QGraphicsItem::ItemIsMovable);
	setZValue(100);
}

Handle::~Handle() {
}


void Handle::setScaleFactor(double factor) {
	d_factor = factor;
	d_inside->setRect(build());
}

void Handle::addToScene(QGraphicsScene * scene) {
	scene->addItem(this);
	if ( scene->metaObject()->className() == std::string("VectorialScene") ) {
		auto vScene = static_cast<VectorialScene*>(scene);
		setScaleFactor(vScene->handleScaleFactor());
		connect(vScene,
		        &VectorialScene::handleScaleFactorChanged,
		        this,
		        &Handle::setScaleFactor);
	}
}

void Handle::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItemGroup::mouseMoveEvent(e);
	d_onMove();
}

void Handle::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	QGraphicsItemGroup::mouseReleaseEvent(e);
	d_onRelease();
}
