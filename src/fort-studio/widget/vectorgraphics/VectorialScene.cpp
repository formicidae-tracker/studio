#include "VectorialScene.hpp"

#include <QGraphicsSceneMouseEvent>

#include "Handle.hpp"
#include "Vector.hpp"
#include "Capsule.hpp"
#include "Polygon.hpp"
#include "Circle.hpp"

#include <iostream>

#include <fort-studio/widget/base/ColorComboBox.hpp>

#include <myrmidon/priv/Capsule.hpp>
#include <fort-studio/Utils.hpp>

VectorialScene::VectorialScene(QObject * parent)
	: QGraphicsScene(parent)
	, d_once(true)
	, d_handleScaleFactor(1.0) {
	setSceneRect(QRectF(0,0,1200,1200));
	setBackgroundBrush(QColor(127,127,127));
	setMode(Mode::Edit);
	d_color = ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(0));

	d_editPressEH = [this](QGraphicsSceneMouseEvent *e) {
		                QGraphicsScene::mousePressEvent(e);
	                };

	d_editMoveEH = [this](QGraphicsSceneMouseEvent *e) {
		               QGraphicsScene::mouseMoveEvent(e);
	               };

	d_editReleaseEH = [this](QGraphicsSceneMouseEvent *e) {
		                  QGraphicsScene::mouseReleaseEvent(e);
	                  };

	d_insertVectorPressEH =
		[this](QGraphicsSceneMouseEvent *e) {
			if ( e->button() != Qt::LeftButton) {
				return;
			}
			auto pos = e->scenePos();
			auto vector = new Vector(pos.x(),pos.y(),pos.x(),pos.y(),d_color);
			vector->addToScene(this);
			connect(vector,
			        &Shape::updated,
			        []() {
				        std::cerr << "vector updated" << std::endl;
		                                });
			d_mouseMove =
				[vector] (QGraphicsSceneMouseEvent *e) {
					vector->setEndPos(e->scenePos());
				};
			d_mouseRelease =
				[vector,this](QGraphicsSceneMouseEvent * e) {
					if ( e->button() != Qt::LeftButton ) {
						return;
					}
					auto pos = e->scenePos();
					vector->setEndPos(pos);
					std::cerr << "vector inserted" << std::endl;
					if ( d_once == true ) {
						setMode(Mode::Edit);
					} else {
						this->d_mouseMove = this->d_editMoveEH;
						this->d_mouseRelease = this->d_editReleaseEH;
					}
				};
		};

	d_insertCapsulePressEH =
		[this] (QGraphicsSceneMouseEvent *e) {
			if ( e->button() != Qt::LeftButton) {
				return;
			}
			auto pos = e->scenePos();
			auto capsule = new Capsule(pos,pos,0,0,d_color);
			capsule->setC2AndRadiusFromPos(pos);
			capsule->addToScene(this);
			connect(capsule,
			        &Shape::updated,
			        []() {
				        std::cerr << "capsule updated" << std::endl;
			        });
			d_mouseMove =
				[capsule] (QGraphicsSceneMouseEvent *e) {
					capsule->setC2AndRadiusFromPos(e->scenePos());
				};
			d_mouseRelease =
				[capsule,this](QGraphicsSceneMouseEvent * e) {
					if ( e->button() != Qt::LeftButton ) {
						return;
					}
					auto pos = e->scenePos();
					capsule->setC2AndRadiusFromPos(pos);
					std::cerr << "capsule inserted" << std::endl;
					if ( d_once == true ) {
						setMode(Mode::Edit);
					} else {
						d_mouseMove = d_editMoveEH;
						d_mouseRelease = d_editReleaseEH;
					}
				};
		};

	d_insertCirclePressEH =
		[this] (QGraphicsSceneMouseEvent *e) {
			if ( e->button() != Qt::LeftButton) {
				return;
			}
			auto pos = e->scenePos();
			auto circle = new Circle(pos,0,d_color);
			circle->setRadiusFromPos(pos);
			circle->addToScene(this);
			connect(circle,
			        &Shape::updated,
			        []() {
				        std::cerr << "circle updated" << std::endl;
			        });
			d_mouseMove =
				[circle] (QGraphicsSceneMouseEvent *e) {
					circle->setRadiusFromPos(e->scenePos());
				};
			d_mouseRelease =
				[circle,this](QGraphicsSceneMouseEvent * e) {
					if ( e->button() != Qt::LeftButton ) {
						return;
					}
					auto pos = e->scenePos();
					circle->setRadiusFromPos(pos);
					std::cerr << "circle inserted" << std::endl;
					if ( d_once == true ) {
						setMode(Mode::Edit);
					} else {
						d_mouseMove = d_editMoveEH;
						d_mouseRelease = d_editReleaseEH;
					}
				};
		};

	d_insertPolygonPressEH =
		[this] (QGraphicsSceneMouseEvent *e) {
			if ( e->button() != Qt::LeftButton ) {
				return;
			}
			auto start = e->scenePos();
			auto polygon = new Polygon({start},d_color);
			polygon->addToScene(this);
			std::cerr << "Starting polygon insertion" << std::endl;
			d_mouseMove = [](QGraphicsSceneMouseEvent * e){ e->ignore();};
			d_mouseRelease = [](QGraphicsSceneMouseEvent * e){ e->ignore();};
			d_mousePress =
				[start,polygon,this](QGraphicsSceneMouseEvent *e) {
					auto newPos = e->scenePos();
					auto dist = (ToEigen(newPos) - ToEigen(start)).norm();

					if ( polygon->vertices().size() > 2
					     && (e->button() == Qt::RightButton
					         || ( e->button() == Qt::LeftButton && dist < Handle::SIZE) ) ) {
						polygon->close();
						std::cerr << "End polygon insertion" << std::endl;
						if ( d_once == true ) {
							setMode(Mode::Edit);
						} else {
							d_mousePress = d_insertPolygonPressEH;
						}
						return;
					}
					if ( e->button() != Qt::LeftButton ) {
						return;
					}

					auto p = polygon->appendPoint(e->scenePos());
					if ( p != nullptr ) {
						p->addToScene(this);
					}
				};
		};

}

VectorialScene::~VectorialScene() {
}


void VectorialScene::setColor(const QColor & color) {
	d_color = color;
}

void VectorialScene::setMode(Mode mode) {
	d_mode = mode;
	switch(d_mode) {
	case Mode::Edit:
		d_mousePress = d_editPressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	case Mode::InsertVector: {
		d_mousePress = d_insertVectorPressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	}
	case Mode::InsertCapsule: {
		d_mousePress = d_insertCapsulePressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	}
	case Mode::InsertCircle: {
		d_mousePress = d_insertCirclePressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	}
	case Mode::InsertPolygon: {
		d_mousePress = d_insertPolygonPressEH;
		d_mouseMove = d_editMoveEH;
		d_mouseRelease = d_editReleaseEH;
		break;
	}

	}

}

void VectorialScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	auto saved = d_mousePress;
	saved(mouseEvent);
}


void VectorialScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	auto saved = d_mouseMove;
	saved(mouseEvent);
}

void VectorialScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	auto saved = d_mouseRelease;
	saved(mouseEvent);
}


double VectorialScene::handleScaleFactor() const {
	return d_handleScaleFactor;
}

void VectorialScene::setHandleScaleFactor(double factor) {
	if ( factor == d_handleScaleFactor ) {
		return;
	}
	d_handleScaleFactor = factor;
	emit handleScaleFactorChanged(d_handleScaleFactor);
}

void VectorialScene::onZoomed(double factor) {
	setHandleScaleFactor(std::max(1.0/factor,1.0));
}
