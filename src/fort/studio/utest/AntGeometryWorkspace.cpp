#include "AntGeometryWorkspace.hpp"

#include <QMainWindow>
#include <QAction>
#include <QComboBox>

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/bridge/AntShapeTypeBridge.hpp>
#include <fort/studio/bridge/AntShapeBridge.hpp>


void WorkspaceUTest::SetUp(const std::string & name) {
	experiment = fmp::Experiment::Create(TestSetup::Basedir() / name);

	bridge = new ExperimentBridge();
	mainWindow = new QMainWindow();
	bridge->setExperiment(experiment);
}

void WorkspaceUTest::TearDown() {
	delete mainWindow;
	delete bridge;
	bridge = nullptr;
}


void AntShapeWorkspaceUTest::SetUp() {
	WorkspaceUTest::SetUp("ant-shape-workspace.myrmidon");
	shapeWs = new AntShapeWorkspace();
	static_cast<Workspace*>(shapeWs)->initialize(mainWindow,bridge);

	bridge->createAnt();
	bridge->identifier()->addIdentification(1,1,nullptr,nullptr);

	bridge->antShapeTypes()->addType("foo");

	shapeWs->setTagCloseUp(std::make_shared<fmp::TagCloseUp>(TestSetup::Basedir() / "",
	                                                         fmp::FrameReference("foo.0000",1,fm::Time()),
	                                                         1,
	                                                         Eigen::Vector2d::Zero(),
	                                                         0.0,
	                                                         fmp::Vector2dList({
		      Eigen::Vector2d(1,1),
		      Eigen::Vector2d(1,-1),
		      Eigen::Vector2d(-1,-1),
		      Eigen::Vector2d(-1,1)})));

	bridge->selectAnt(1);

}

void AntShapeWorkspaceUTest::TearDown() {
	delete shapeWs;
	WorkspaceUTest::TearDown();
}


TEST_F(AntShapeWorkspaceUTest,AddCapsuleActionState) {
	shapeWs->typeBox()->setCurrentIndex(-1);
	EXPECT_EQ(shapeWs->insertAction()->isEnabled(),false);
	shapeWs->typeBox()->setCurrentIndex(0);
	EXPECT_EQ(shapeWs->insertAction()->isEnabled(),true);
}

TEST_F(AntShapeWorkspaceUTest,CloneShapeAction) {
	EXPECT_EQ(shapeWs->cloneAntShapeAction()->isEnabled(),false) << "Should be disabled if current ant has no shape";
	bridge->antShapes()->addCapsule(1,1,std::make_shared<fmp::Capsule>(Eigen::Vector2d(0,-1),
	                                                                   Eigen::Vector2d(0,1),
	                                                                   0.3,0.3));
	EXPECT_EQ(shapeWs->cloneAntShapeAction()->isEnabled(),true) << "Should be enabled if current ant has a shape";
}
