#pragma once

#include <QWidget>

#include <fort/studio/bridge/ZoneBridge.hpp>

#include "Navigatable.hpp"

#include <fort/studio/widget/vectorgraphics/VectorialScene.hpp>

namespace Ui {
class ZoningWidget;
}
class ExperimentBridge;
class QAction;

class ZoningWidget : public QWidget , public Navigatable{
	Q_OBJECT
public:
	explicit ZoningWidget(QWidget *parent = 0);
	~ZoningWidget();

	void setup(ExperimentBridge * experiment);

protected:
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;

public slots:
	void nextCloseUp();
	void previousCloseUp();

private slots:
	void onCopyTime();

	void onNewZoneDefinition(QList<ZoneDefinitionBridge*> bridges);


	void setSceneMode(VectorialScene::Mode mode);

	void onShapeCreated(QSharedPointer<Shape> shape);
	void onShapeRemoved(QSharedPointer<Shape> shape);


private:
	void display(const std::shared_ptr<ZoneBridge::FullFrame> & fullframe);

	void select(int increment);

	Ui::ZoningWidget * d_ui;
	ZoneBridge       * d_zones;

	void appendShape(const fmp::Shape::ConstPtr & shape,
	                 fmp::Zone::ID zID);
	void rebuildGeometry(const QSharedPointer<Shape> & shape );
	void rebuildGeometry(fmp::Zone::ID zID );

	fmp::Zone::ID currentZoneID() const;

	static fmp::Shape::ConstPtr convertShape(const QSharedPointer<Shape> & s);

	std::shared_ptr<ZoneBridge::FullFrame> d_fullframe;
	QAction                              * d_copyAction;
	VectorialScene                       * d_vectorialScene;

	std::map<fmp::Zone::ID,ZoneDefinitionBridge*> d_definitions;
	std::map<QSharedPointer<Shape>,fmp::Zone::ID> d_shapes;
};
