#pragma once

#include <fort/studio/bridge/ZoneBridge.hpp>

#include "Workspace.hpp"

#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>

namespace Ui {
class ZoningWorkspace;
}

class QToolBar;
class QComboBox;
class ExperimentBridge;
class QAction;
class QDockWidget;
class QListView;

class ZoningWorkspace : public Workspace{
	Q_OBJECT
public:
	explicit ZoningWorkspace(QWidget *parent = 0);
	virtual ~ZoningWorkspace();


protected:
	void initialize(QMainWindow * main, ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;

public slots:
	void nextCloseUp();
	void previousCloseUp();

private slots:
	void onCopyTime();

	void onNewZoneDefinition(QList<ZoneDefinitionBridge*> bridges);

	void onSceneModeChanged(VectorialScene::Mode mode);

	void setSceneMode(VectorialScene::Mode mode);

	void onShapeCreated(QSharedPointer<Shape> shape);
	void onShapeRemoved(QSharedPointer<Shape> shape);


	void onComboBoxCurrentIndexChanged(int);

private:
	void setUpUI();

	void display(const std::shared_ptr<ZoneBridge::FullFrame> & fullframe);

	void select(int increment);

	Ui::ZoningWorkspace * d_ui;
	ZoneBridge       * d_zones;

	void appendShape(const fmp::Shape::Ptr & shape,
	                 fm::ZoneID zID);
	void rebuildGeometry(const QSharedPointer<Shape> & shape );
	void rebuildGeometry(fm::ZoneID zID );

	fm::ZoneID currentZoneID() const;

	static fm::Shape::Ptr convertShape(const QSharedPointer<Shape> & s);

	void changeShapeType(Shape * shape, fm::ZoneID zID);

	void setUpFullFrameLabels(const std::shared_ptr<ZoneBridge::FullFrame> & fullFrame);


	QAction   * d_copyAction;
	QAction   * d_editAction;
	QAction   * d_circleAction;
	QAction   * d_capsuleAction;
	QAction   * d_polygonAction;
	QComboBox * d_comboBox;
	QToolBar  * d_toolBar;

	QListView   * d_listView;
	QDockWidget * d_fullFramesDock;

	std::shared_ptr<ZoneBridge::FullFrame> d_fullframe;
	VectorialScene                       * d_vectorialScene;



	std::map<fm::ZoneID,ZoneDefinitionBridge*> d_definitions;
	std::map<QSharedPointer<Shape>,fm::ZoneID> d_shapes;
};
