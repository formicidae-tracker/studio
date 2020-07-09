#include "ZoningWidget.hpp"
#include "ui_ZoningWidget.h"

#include <QAction>
#include <QClipboard>

#include <fort/myrmidon/priv/Polygon.hpp>
#include <fort/myrmidon/priv/Circle.hpp>
#include <fort/myrmidon/priv/Capsule.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>

#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>
#include <fort/studio/widget/vectorgraphics/Polygon.hpp>
#include <fort/studio/widget/vectorgraphics/Circle.hpp>
#include <fort/studio/widget/vectorgraphics/Capsule.hpp>

#include <fort/studio/Format.hpp>
#include <fort/studio/MyrmidonTypes/Conversion.hpp>


ZoningWidget::ZoningWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::ZoningWidget)
	, d_zones(nullptr)
	, d_vectorialScene(new VectorialScene(this)) {
	d_ui->setupUi(this);


	d_ui->vectorialView->setScene(d_vectorialScene);
	d_ui->vectorialView->setRenderHint(QPainter::Antialiasing,true);
    connect(d_ui->vectorialView,
            &VectorialView::zoomed,
            d_vectorialScene,
            &VectorialScene::onZoomed);


    d_ui->editButton->setCheckable(true);
    connect(d_ui->editButton,&QToolButton::clicked,
            this,[this](){ setSceneMode(VectorialScene::Mode::Edit); });
    d_ui->polygonButton->setCheckable(true);
    connect(d_ui->polygonButton,&QToolButton::clicked,
            this,[this](){ setSceneMode(VectorialScene::Mode::InsertPolygon); });
    d_ui->circleButton->setCheckable(true);
    connect(d_ui->circleButton,&QToolButton::clicked,
            this,[this](){ setSceneMode(VectorialScene::Mode::InsertCircle); });
    d_ui->capsuleButton->setCheckable(true);
    connect(d_ui->capsuleButton,&QToolButton::clicked,
            this,[this](){ setSceneMode(VectorialScene::Mode::InsertCapsule); });

    d_ui->editButton->setChecked(Qt::Checked);
    connect(d_vectorialScene,&VectorialScene::modeChanged,
            this,&ZoningWidget::onSceneModeChanged);


    connect(d_vectorialScene,&VectorialScene::polygonCreated,
            this,&ZoningWidget::onShapeCreated);
    connect(d_vectorialScene,&VectorialScene::polygonRemoved,
            this,&ZoningWidget::onShapeRemoved);

    connect(d_vectorialScene,&VectorialScene::circleCreated,
            this,&ZoningWidget::onShapeCreated);
    connect(d_vectorialScene,&VectorialScene::circleRemoved,
            this,&ZoningWidget::onShapeRemoved);

    connect(d_vectorialScene,&VectorialScene::capsuleCreated,
            this,&ZoningWidget::onShapeCreated);
    connect(d_vectorialScene,&VectorialScene::capsuleRemoved,
            this,&ZoningWidget::onShapeRemoved);


}

ZoningWidget::~ZoningWidget() {
	delete d_ui;
}

void ZoningWidget::setup(ExperimentBridge * experiment) {
	d_zones = experiment->zones();
	d_ui->zonesEditor->setup(d_zones);
	d_ui->listView->setModel(d_zones->fullFrameModel());
	d_ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
	auto sModel = d_ui->listView->selectionModel();
	connect(sModel,&QItemSelectionModel::selectionChanged,
	        this,
	        [this,sModel]() {
		        if ( sModel->hasSelection() == false ) {
			        display(nullptr);
		        }
		        auto f = d_zones->fullFrameAtIndex(sModel->selectedIndexes()[0]);
		        if ( f.first == false ) {
			        display(nullptr);
		        } else {
			        display(std::make_shared<ZoneBridge::FullFrame>(f.second));
		        }
	        });
	connect(d_zones,&ZoneBridge::newZoneDefinitionBridge,
	        this,&ZoningWidget::onNewZoneDefinition);
}


void ZoningWidget::display(const std::shared_ptr<ZoneBridge::FullFrame> & fullframe) {
	d_fullframe = fullframe;
	if ( d_copyAction != nullptr ) {
		d_copyAction->setEnabled(!fullframe == false);
	}

	if ( !d_fullframe == true ) {
		d_vectorialScene->clearPolygons();
		d_vectorialScene->clearCircles();
		d_vectorialScene->clearCapsules();
		d_vectorialScene->setBackgroundPicture("");
		return;
	}

	d_vectorialScene->setBackgroundPicture(d_fullframe->AbsoluteFilePath);
	d_ui->vectorialView->showEntireScene();

	d_zones->selectTime(fullframe->Reference.Time());
}


void ZoningWidget::setUp(const NavigationAction & actions) {
	connect(actions.NextCloseUp,&QAction::triggered,
	        this,&ZoningWidget::nextCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        this,&ZoningWidget::previousCloseUp);

	connect(actions.CopyCurrentTime,&QAction::triggered,
	        this,&ZoningWidget::onCopyTime);

	actions.NextCloseUp->setEnabled(true);
	actions.PreviousCloseUp->setEnabled(true);
	actions.CopyCurrentTime->setEnabled(!d_fullframe == false);
	d_copyAction = actions.CopyCurrentTime;
}

void ZoningWidget::tearDown(const NavigationAction & actions) {
	disconnect(actions.NextCloseUp,&QAction::triggered,
	           this,&ZoningWidget::nextCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           this,&ZoningWidget::previousCloseUp);

	disconnect(actions.CopyCurrentTime,&QAction::triggered,
	           this,&ZoningWidget::onCopyTime);

	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);
	actions.CopyCurrentTime->setEnabled(false);
	d_copyAction = nullptr;
}


void ZoningWidget::nextCloseUp() {
	select(+1);
}

void ZoningWidget::previousCloseUp() {
	select(-1);
}

void ZoningWidget::onCopyTime() {
	if ( !d_fullframe == true ) {
		return;
	}
	QApplication::clipboard()->setText(ToQString(d_fullframe->Reference.Time()));
}


void ZoningWidget::select(int increment) {
	if ( d_zones->fullFrameModel()->rowCount() == 0 || increment == 0 ) {
		return;
	}
	auto model = d_zones->fullFrameModel();
	auto sModel = d_ui->listView->selectionModel();
	auto rows = sModel->selectedRows();
	if ( rows.isEmpty() == true ) {
		if ( increment > 0 ) {
			sModel->select(model->index(0,0),QItemSelectionModel::ClearAndSelect);
		} else {
			sModel->select(model->index(model->rowCount()-1,0),QItemSelectionModel::ClearAndSelect);
		}
		return;
	}

	int currentRow = rows[0].row();
	int newRow = std::clamp(currentRow + increment,0,model->rowCount()-1);
	if ( currentRow == newRow ) {
		return;
	}
	sModel->select(model->index(newRow,0),QItemSelectionModel::ClearAndSelect);
}


void ZoningWidget::onShapeCreated(QSharedPointer<Shape> shape) {
	auto zoneID = currentZoneID();
	auto fmShape = convertShape(shape);

	if ( !d_fullframe == true
	     || zoneID == 0
	     || !fmShape == true) {
		d_vectorialScene->deleteShape(shape);
		return;
	}

	connect(shape.data(),&Shape::updated,
	        this,[this,shape]{ rebuildGeometry(shape); });
	d_shapes.insert(std::make_pair(shape,zoneID));

	rebuildGeometry(shape);
}

void ZoningWidget::onShapeRemoved(QSharedPointer<Shape> shape) {
	auto fi = d_shapes.find(shape);
	if ( !d_fullframe == true
	     || fi == d_shapes.end() ) {
		return;
	}
	auto zoneID = fi->second;
	d_shapes.erase(fi);
	rebuildGeometry(zoneID);
}


void ZoningWidget::setSceneMode(VectorialScene::Mode mode) {
	if ( !d_fullframe == true || d_ui->comboBox->count() == 0 ) {
		d_vectorialScene->setMode(VectorialScene::Mode::Edit);
		onSceneModeChanged(VectorialScene::Mode::Edit);
		return;
	}
	d_vectorialScene->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(currentZoneID())));
	d_vectorialScene->setMode(mode);
}


void ZoningWidget::onNewZoneDefinition(QList<ZoneDefinitionBridge*> bridges) {
	d_vectorialScene->clearCapsules();
	d_vectorialScene->clearPolygons();
	d_vectorialScene->clearCircles();

	d_shapes.clear();
	d_definitions.clear();

	d_ui->comboBox->clear();

	if ( bridges.isEmpty() == true ) {
		setSceneMode(VectorialScene::Mode::Edit);
		return;
	}

	for ( const auto & b : bridges ) {\
		auto zoneID = b->zone().ZoneID();
		d_definitions.insert(std::make_pair(zoneID,b));
		auto colorFM = fmp::Palette::Default().At(zoneID);
		auto color = Conversion::colorFromFM(colorFM);
		d_ui->comboBox->addItem(Conversion::iconFromFM(colorFM),
		                        ToQString(b->zone().Name()),
		                        quint32(zoneID));

		d_vectorialScene->setColor(color);
		for ( const auto & s : b->geometry().Shapes() ) {
			appendShape(s,zoneID);
		}

	}
}


void ZoningWidget::appendShape(const fmp::Shape::ConstPtr & s,
                               fmp::Zone::ID zoneID) {
	QSharedPointer<Shape> newShape;
	if ( auto p = std::dynamic_pointer_cast<const fmp::Polygon>(s) ) {
		if ( p->Size() > 2 ) {
			QVector<QPointF> vertices;
			for ( size_t i = 0; i < p->Size(); ++i ) {
				vertices.push_back(Conversion::fromEigen(p->Vertex(i)));
			}
			vertices.push_back(Conversion::fromEigen(p->Vertex(0)));
			newShape = d_vectorialScene->appendPolygon(vertices);
		}
	}

	if ( auto c = std::dynamic_pointer_cast<const fmp::Circle>(s) ) {
		newShape = d_vectorialScene->appendCircle(Conversion::fromEigen(c->Center()),c->Radius());
	}

	if ( auto c = std::dynamic_pointer_cast<const fmp::Capsule>(s) ) {
		newShape = d_vectorialScene->appendCapsule(Conversion::fromEigen(c->C1()),
		                                           Conversion::fromEigen(c->C2()),
		                                           c->R1(),
		                                           c->R2());
	}

	if ( !newShape ) {
		return;
	}

	connect(newShape.data(),&Shape::updated,
	        this,[this,newShape]{
		             rebuildGeometry(newShape);
	             });
	d_shapes.insert(std::make_pair(newShape,zoneID));
}


void ZoningWidget::rebuildGeometry(const QSharedPointer<Shape> & shape ) {
	auto fi = d_shapes.find(shape);
	if ( fi == d_shapes.end() ) {
		return;
	}

	rebuildGeometry(fi->second);
}

void ZoningWidget::rebuildGeometry(fmp::Zone::ID zoneID ) {
	auto fi = d_definitions.find(zoneID);
	if ( fi == d_definitions.end()) {
		return;
	}
	std::vector<fmp::Shape::ConstPtr> shapes;

	for ( const auto & [shape,zoneID_] : d_shapes ) {
		if ( zoneID != zoneID_) {
			continue;
		}
		shapes.push_back(convertShape(shape));
	}
	fi->second->setGeometry(shapes);
}

fmp::Shape::ConstPtr ZoningWidget::convertShape(const QSharedPointer<Shape> & s) {
	if ( auto p = s.dynamicCast<Polygon>() ) {
		fm::Vector2dList vertices;
		for ( const auto & v : p->vertices() ) {
			vertices.push_back(Conversion::toEigen(v));
		}
		return std::make_shared<fmp::Polygon>(vertices);
	}

	if ( auto c = s.dynamicCast<Circle>() ) {
		return std::make_shared<fmp::Circle>(Conversion::toEigen(c->pos()),c->radius());
	}

	if ( auto c = s.dynamicCast<Capsule>() ) {
		return std::make_shared<fmp::Capsule>(Conversion::toEigen(c->c1Pos()),
		                                      Conversion::toEigen(c->c2Pos()),
		                                      c->r1(),
		                                      c->r2());
	}

	return fmp::Shape::ConstPtr();
}

fmp::Zone::ID ZoningWidget::currentZoneID() const {
	if ( d_ui->comboBox->count() == 0 || d_ui->comboBox->currentIndex() < 0 ) {
		return 0;
	}
	return d_ui->comboBox->currentData().toInt();
}


void ZoningWidget::onSceneModeChanged(VectorialScene::Mode mode) {
	d_ui->editButton->setChecked(Qt::Unchecked);
	d_ui->polygonButton->setChecked(Qt::Unchecked);
	d_ui->circleButton->setChecked(Qt::Unchecked);
	d_ui->capsuleButton->setChecked(Qt::Unchecked);
	switch(mode) {
	case VectorialScene::Mode::Edit:
		d_ui->editButton->setChecked(Qt::Checked);
		break;
	case VectorialScene::Mode::InsertPolygon:
		d_ui->polygonButton->setChecked(Qt::Checked);
		break;
	case VectorialScene::Mode::InsertCircle:
		d_ui->circleButton->setChecked(Qt::Checked);
		break;
	case VectorialScene::Mode::InsertCapsule:
		d_ui->capsuleButton->setChecked(Qt::Checked);
		break;
	default:
		break;
	}
}


void ZoningWidget::on_comboBox_currentIndexChanged(int) {
	auto zoneID = currentZoneID();

	d_vectorialScene->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(zoneID)));

	for ( const auto item : d_vectorialScene->selectedItems() ) {
		if ( auto v = dynamic_cast<Shape*>(item) ) {
			changeShapeType(v,zoneID);
		}
	}
}


void ZoningWidget::changeShapeType(Shape * shape, fmp::Zone::ID zoneID) {
	auto fi = std::find_if(d_shapes.begin(),
	                       d_shapes.end(),
	                       [shape](const std::pair<QSharedPointer<Shape>,fmp::Zone::ID> & iter ) -> bool {
		                       return iter.first.data() == shape;
	                       });
	if ( fi == d_shapes.end() ) {
		return;
	}
	auto oldZoneID = fi->second;
	fi->second = zoneID;
	fi->first->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(zoneID)));
	rebuildGeometry(zoneID);
	rebuildGeometry(oldZoneID);

}
