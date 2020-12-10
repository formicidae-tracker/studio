#include "ZoningWorkspace.hpp"
#include "ui_ZoningWorkspace.h"

#include <QAction>
#include <QClipboard>
#include <QToolBar>
#include <QComboBox>
#include <QLabel>
#include <QListView>
#include <QDockWidget>

#include <QMainWindow>

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


void ZoningWorkspace::setUpUI() {
	d_toolBar = new QToolBar(this);
	d_editAction = d_toolBar->addAction(QIcon(":/icons/cursor.svg"),
	                                    tr("Edit Shapes"));
	d_editAction->setToolTip(tr("Transform and move shapes"));
	d_editAction->setStatusTip(d_editAction->toolTip());
	d_polygonAction = d_toolBar->addAction(QIcon(":/icons/polygon.svg"),
	                                    tr("Insert polygon"));
	d_polygonAction->setToolTip(tr("Insert a polygon (right-click to finish edition)"));
	d_polygonAction->setStatusTip(d_polygonAction->toolTip());

	d_circleAction = d_toolBar->addAction(QIcon(":/icons/circle.svg"),
	                                      tr("Insert circle"));
	d_circleAction->setToolTip(tr("Insert a circle"));
	d_circleAction->setStatusTip(d_circleAction->toolTip());

	d_capsuleAction = d_toolBar->addAction(QIcon(":/icons/capsule.svg"),
	                                       tr("Insert capsule"));

	d_capsuleAction->setToolTip(tr("Insert a capsule"));
	d_capsuleAction->setStatusTip(d_capsuleAction->toolTip());


	d_toolBar->addWidget(new QLabel(tr("Zone:"),this));
	d_comboBox = new QComboBox(this);
	d_comboBox->setObjectName("comboBox");
	d_comboBox->setMinimumSize(QSize(200,0));
	d_toolBar->addWidget(d_comboBox);
	connect(d_comboBox,qOverload<int>(&QComboBox::currentIndexChanged),
	        this,&ZoningWorkspace::onComboBoxCurrentIndexChanged);

	auto widget = new QWidget(this);
	auto layout = new QVBoxLayout();

	d_listView = new QListView(widget);
	d_listView->setMaximumSize(QSize(250,65535));
	layout->addWidget(d_listView);

	widget->setLayout(layout);

	d_fullFramesDock = new QDockWidget(tr("Space's Full-Frames"));
	d_fullFramesDock->setWidget(widget);


}

ZoningWorkspace::ZoningWorkspace(QWidget *parent)
	: Workspace(false,parent)
	, d_ui(new Ui::ZoningWorkspace)
	, d_zones(nullptr)
	, d_vectorialScene(new VectorialScene(this)) {
	setUpUI();
	d_ui->setupUi(this);


	d_ui->vectorialView->setScene(d_vectorialScene);
	d_ui->vectorialView->setRenderHint(QPainter::Antialiasing,true);
    connect(d_ui->vectorialView,
            &VectorialView::zoomed,
            d_vectorialScene,
            &VectorialScene::onZoomed);


    d_editAction->setCheckable(true);
    connect(d_editAction,&QAction::triggered,
            this,[this](){ setSceneMode(VectorialScene::Mode::Edit); });
    d_polygonAction->setCheckable(true);
    connect(d_polygonAction,&QAction::triggered,
            this,[this](){ setSceneMode(VectorialScene::Mode::InsertPolygon); });
    d_circleAction->setCheckable(true);
    connect(d_circleAction,&QAction::triggered,
            this,[this](){ setSceneMode(VectorialScene::Mode::InsertCircle); });
    d_capsuleAction->setCheckable(true);
    connect(d_capsuleAction,&QAction::triggered,
            this,[this](){ setSceneMode(VectorialScene::Mode::InsertCapsule); });

    d_editAction->setChecked(Qt::Checked);
    connect(d_vectorialScene,&VectorialScene::modeChanged,
            this,&ZoningWorkspace::onSceneModeChanged);

    connect(d_vectorialScene,&VectorialScene::polygonCreated,
            this,&ZoningWorkspace::onShapeCreated);
    connect(d_vectorialScene,&VectorialScene::polygonRemoved,
            this,&ZoningWorkspace::onShapeRemoved);

    connect(d_vectorialScene,&VectorialScene::circleCreated,
            this,&ZoningWorkspace::onShapeCreated);
    connect(d_vectorialScene,&VectorialScene::circleRemoved,
            this,&ZoningWorkspace::onShapeRemoved);

    connect(d_vectorialScene,&VectorialScene::capsuleCreated,
            this,&ZoningWorkspace::onShapeCreated);
    connect(d_vectorialScene,&VectorialScene::capsuleRemoved,
            this,&ZoningWorkspace::onShapeRemoved);

	setUpFullFrameLabels(nullptr);

}

ZoningWorkspace::~ZoningWorkspace() {
	delete d_ui;
}


void ZoningWorkspace::initialize(QMainWindow * main, ExperimentBridge * experiment) {
	d_zones = experiment->zones();
	d_ui->zonesEditor->setup(d_zones);
	d_listView->setModel(d_zones->fullFrameModel());
	d_listView->setSelectionMode(QAbstractItemView::SingleSelection);
	d_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
	auto sModel = d_listView->selectionModel();
	connect(d_zones->fullFrameModel(),&QAbstractItemModel::modelReset,
	        this,[this]() {
		        display(nullptr);
	        });

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
	        this,&ZoningWorkspace::onNewZoneDefinition);

	main->addToolBar(d_toolBar);
	d_toolBar->hide();

	main->addDockWidget(Qt::LeftDockWidgetArea,d_fullFramesDock);
	d_fullFramesDock->hide();
}


void ZoningWorkspace::display(const std::shared_ptr<ZoneBridge::FullFrame> & fullframe) {
	d_fullframe = fullframe;
	if ( d_copyAction != nullptr ) {
		d_copyAction->setEnabled(!fullframe == false);
	}

	setUpFullFrameLabels(fullframe);

	if ( d_fullframe == nullptr ) {
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


void ZoningWorkspace::setUp(const NavigationAction & actions) {
	connect(actions.NextCloseUp,&QAction::triggered,
	        this,&ZoningWorkspace::nextCloseUp);
	connect(actions.PreviousCloseUp,&QAction::triggered,
	        this,&ZoningWorkspace::previousCloseUp);

	connect(actions.CopyCurrentTime,&QAction::triggered,
	        this,&ZoningWorkspace::onCopyTime);

	actions.NextCloseUp->setEnabled(true);
	actions.PreviousCloseUp->setEnabled(true);
	actions.CopyCurrentTime->setEnabled(!d_fullframe == false);
	d_copyAction = actions.CopyCurrentTime;
	d_toolBar->show();

	d_fullFramesDock->show();
}

void ZoningWorkspace::tearDown(const NavigationAction & actions) {
	disconnect(actions.NextCloseUp,&QAction::triggered,
	           this,&ZoningWorkspace::nextCloseUp);
	disconnect(actions.PreviousCloseUp,&QAction::triggered,
	           this,&ZoningWorkspace::previousCloseUp);

	disconnect(actions.CopyCurrentTime,&QAction::triggered,
	           this,&ZoningWorkspace::onCopyTime);

	actions.NextCloseUp->setEnabled(false);
	actions.PreviousCloseUp->setEnabled(false);
	actions.CopyCurrentTime->setEnabled(false);
	d_copyAction = nullptr;
	d_toolBar->hide();

	d_fullFramesDock->hide();
}


void ZoningWorkspace::nextCloseUp() {
	select(+1);
}

void ZoningWorkspace::previousCloseUp() {
	select(-1);
}

void ZoningWorkspace::onCopyTime() {
	if ( !d_fullframe == true ) {
		return;
	}
	QApplication::clipboard()->setText(ToQString(d_fullframe->Reference.Time()));
}


void ZoningWorkspace::select(int increment) {
	if ( d_zones->fullFrameModel()->rowCount() == 0 || increment == 0 ) {
		return;
	}
	auto model = d_zones->fullFrameModel();
	auto sModel = d_listView->selectionModel();
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


void ZoningWorkspace::onShapeCreated(QSharedPointer<Shape> shape) {
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

void ZoningWorkspace::onShapeRemoved(QSharedPointer<Shape> shape) {
	auto fi = d_shapes.find(shape);
	if ( !d_fullframe == true
	     || fi == d_shapes.end() ) {
		return;
	}
	auto zoneID = fi->second;
	d_shapes.erase(fi);
	rebuildGeometry(zoneID);
}


void ZoningWorkspace::setSceneMode(VectorialScene::Mode mode) {
	if ( !d_fullframe == true || d_comboBox->count() == 0 ) {
		d_vectorialScene->setMode(VectorialScene::Mode::Edit);
		onSceneModeChanged(VectorialScene::Mode::Edit);
		return;
	}
	d_vectorialScene->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(currentZoneID())));
	d_vectorialScene->setMode(mode);
}


void ZoningWorkspace::onNewZoneDefinition(QList<ZoneDefinitionBridge*> bridges) {
	d_vectorialScene->clearCapsules();
	d_vectorialScene->clearPolygons();
	d_vectorialScene->clearCircles();

	d_shapes.clear();
	d_definitions.clear();

	d_comboBox->clear();

	if ( bridges.isEmpty() == true ) {
		setSceneMode(VectorialScene::Mode::Edit);
		return;
	}

	for ( const auto & b : bridges ) {\
		auto zoneID = b->zone().ZoneID();
		d_definitions.insert(std::make_pair(zoneID,b));
		auto colorFM = fmp::Palette::Default().At(zoneID);
		auto color = Conversion::colorFromFM(colorFM);
		d_comboBox->addItem(Conversion::iconFromFM(colorFM),
		                    ToQString(b->zone().Name()),
		                    quint32(zoneID));

		d_vectorialScene->setColor(color);
		for ( const auto & s : b->geometry().Shapes() ) {
			appendShape(s,zoneID);
		}

	}
}


void ZoningWorkspace::appendShape(const fmp::Shape::ConstPtr & s,
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


void ZoningWorkspace::rebuildGeometry(const QSharedPointer<Shape> & shape ) {
	auto fi = d_shapes.find(shape);
	if ( fi == d_shapes.end() ) {
		return;
	}

	rebuildGeometry(fi->second);
}

void ZoningWorkspace::rebuildGeometry(fmp::Zone::ID zoneID ) {
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

fmp::Shape::ConstPtr ZoningWorkspace::convertShape(const QSharedPointer<Shape> & s) {
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

fmp::Zone::ID ZoningWorkspace::currentZoneID() const {
	if ( d_comboBox->count() == 0 || d_comboBox->currentIndex() < 0 ) {
		return 0;
	}
	return d_comboBox->currentData().toInt();
}


void ZoningWorkspace::onSceneModeChanged(VectorialScene::Mode mode) {
	d_editAction->setChecked(Qt::Unchecked);
	d_polygonAction->setChecked(Qt::Unchecked);
	d_circleAction->setChecked(Qt::Unchecked);
	d_capsuleAction->setChecked(Qt::Unchecked);
	switch(mode) {
	case VectorialScene::Mode::Edit:
		d_editAction->setChecked(Qt::Checked);
		break;
	case VectorialScene::Mode::InsertPolygon:
		d_polygonAction->setChecked(Qt::Checked);
		break;
	case VectorialScene::Mode::InsertCircle:
		d_circleAction->setChecked(Qt::Checked);
		break;
	case VectorialScene::Mode::InsertCapsule:
		d_capsuleAction->setChecked(Qt::Checked);
		break;
	default:
		break;
	}
}


void ZoningWorkspace::onComboBoxCurrentIndexChanged(int) {
	auto zoneID = currentZoneID();

	d_vectorialScene->setColor(Conversion::colorFromFM(fmp::Palette::Default().At(zoneID)));

	for ( const auto item : d_vectorialScene->selectedItems() ) {
		if ( auto v = dynamic_cast<Shape*>(item) ) {
			changeShapeType(v,zoneID);
		}
	}
}


void ZoningWorkspace::changeShapeType(Shape * shape, fmp::Zone::ID zoneID) {
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


void ZoningWorkspace::setUpFullFrameLabels(const std::shared_ptr<ZoneBridge::FullFrame> & fullframe) {
	auto time = tr("N.A.");
	QString URI = time;
	if ( fullframe != nullptr ) {
		time = ToQString(fullframe->Reference.Time());
		URI = fullframe->Reference.URI().c_str();
	}
	d_ui->uriLabel->setText(tr("URI: %1").arg(URI));
	d_ui->timeLabel->setText(tr("Time: %1").arg(time));
}
