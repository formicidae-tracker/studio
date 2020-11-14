#pragma once

#include <fort/studio/bridge/ZoneBridge.hpp>

#include "Workspace.hpp"

#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>

namespace Ui {
class ZoningWorkspace;
}
class ExperimentBridge;
class QAction;

class ZoningWorkspace : public Workspace{
	Q_OBJECT
public:
	explicit ZoningWorkspace(QWidget *parent = 0);
	virtual ~ZoningWorkspace();


protected:
	void initialize(ExperimentBridge * experiment) override;
	void setUp(QMainWindow * main, const NavigationAction & actions) override;
	void tearDown(QMainWindow * main, const NavigationAction & actions) override;

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


	void on_comboBox_currentIndexChanged(int);

private:
	void display(const std::shared_ptr<ZoneBridge::FullFrame> & fullframe);

	void select(int increment);

	Ui::ZoningWorkspace * d_ui;
	ZoneBridge       * d_zones;

	void appendShape(const fmp::Shape::ConstPtr & shape,
	                 fmp::Zone::ID zID);
	void rebuildGeometry(const QSharedPointer<Shape> & shape );
	void rebuildGeometry(fmp::Zone::ID zID );

	fmp::Zone::ID currentZoneID() const;

	static fmp::Shape::ConstPtr convertShape(const QSharedPointer<Shape> & s);

	void changeShapeType(Shape * shape, fmp::Zone::ID zID);


	std::shared_ptr<ZoneBridge::FullFrame> d_fullframe;
	QAction                              * d_copyAction;
	VectorialScene                       * d_vectorialScene;

	std::map<fmp::Zone::ID,ZoneDefinitionBridge*> d_definitions;
	std::map<QSharedPointer<Shape>,fmp::Zone::ID> d_shapes;
};
