#include "AntShapeUTest.hpp"


#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/bridge/AntShapeBridge.hpp>
#include <fort/studio/bridge/AntShapeTypeBridge.hpp>

#include <QAbstractItemModel>
#include <QStandardItemModel>

#include <QDebug>
#include <QSignalSpy>

void AntShapeUTest::SetUp() {
	auto experiment = fmp::Experiment::Create(TestSetup::Basedir() / "ant-measurement-bridge.myrmidon");

	d_experiment.setExperiment(experiment);
}

void AntShapeUTest::TearDown() {
	d_experiment.setExperiment(nullptr);
}


TEST_F(AntShapeUTest,ColumnAreInOrder) {
	auto m = d_experiment.antShapes()->model();
	EXPECT_EQ(m->rowCount(),0);
	EXPECT_EQ(m->columnCount(),1);
	EXPECT_EQ(std::string(m->headerData(0,Qt::Horizontal).toString().toUtf8().constData()),
	          "Ant");

	d_experiment.antShapeTypes()->addType("head");
	d_experiment.antShapeTypes()->addType("thorax");
	d_experiment.antShapeTypes()->addType("legs");

	EXPECT_EQ(m->columnCount(),4);
	EXPECT_EQ(std::string(m->headerData(1,Qt::Horizontal).toString().toUtf8().constData()),
	          "head");

	EXPECT_EQ(std::string(m->headerData(2,Qt::Horizontal).toString().toUtf8().constData()),
	          "thorax");

	EXPECT_EQ(std::string(m->headerData(3,Qt::Horizontal).toString().toUtf8().constData()),
	          "legs");
	auto tModel = d_experiment.antShapeTypes()->shapeModel();
	tModel->setData(tModel->index(2,0),"antennas");
	EXPECT_EQ(std::string(m->headerData(3,Qt::Horizontal).toString().toUtf8().constData()),
	          "antennas");

	d_experiment.antShapeTypes()->deleteType(2);
	EXPECT_EQ(m->columnCount(),3);
	EXPECT_EQ(std::string(m->headerData(2,Qt::Horizontal).toString().toUtf8().constData()),
	          "antennas");

	d_experiment.antShapeTypes()->addType("sterm");
	EXPECT_EQ(m->columnCount(),4);
	EXPECT_EQ(std::string(m->headerData(2,Qt::Horizontal).toString().toUtf8().constData()),
	          "sterm");

	EXPECT_EQ(std::string(m->headerData(3,Qt::Horizontal).toString().toUtf8().constData()),
	          "antennas");
}


TEST_F(AntShapeUTest,ShapesAreCounted) {
	QSignalSpy capsuleAdded(d_experiment.antShapes(),SIGNAL(capsuleCreated(quint32,quint32,quint32,const fm::Capsule::Ptr&)));
	QSignalSpy capsuleCleared(d_experiment.antShapes(),SIGNAL(capsuleCleared(quint32)));

	auto m = d_experiment.antShapes()->model();
	EXPECT_EQ(m->rowCount(),0);
	d_experiment.createAnt();
	EXPECT_EQ(m->rowCount(),1);

	d_experiment.antShapeTypes()->addType("body");
	d_experiment.antShapeTypes()->addType("antenna");

	EXPECT_EQ(m->data(m->index(0,1)).toInt(),
	          0);

	auto capsule = std::make_shared<fmp::Capsule>(Eigen::Vector2d(0,0),
	                                              Eigen::Vector2d(0,1),
	                                              0.1,
	                                              0.1);


	d_experiment.antShapes()->addCapsule(1,
	                                     1,
	                                     capsule);

	EXPECT_EQ(capsuleAdded.count(),1);

	EXPECT_EQ(m->data(m->index(0,1)).toInt(),
	          1);

	d_experiment.antShapes()->addCapsule(1,
	                                     1,
	                                     capsule);

	EXPECT_EQ(capsuleAdded.count(),2);

	EXPECT_EQ(m->data(m->index(0,1)).toInt(),
	          2);


	d_experiment.antShapes()->addCapsule(1,
	                                     2,
	                                     capsule);
	EXPECT_EQ(capsuleAdded.count(),3);

	EXPECT_EQ(m->data(m->index(0,2)).toInt(),
	          1);

	d_experiment.createAnt();

	EXPECT_EQ(m->data(m->index(1,1)).toInt(),
	          0);

	EXPECT_EQ(m->data(m->index(1,2)).toInt(),
	          0);


	d_experiment.antShapes()->cloneShape(1,false,false);

	EXPECT_EQ(capsuleAdded.count(),6);

	EXPECT_EQ(m->data(m->index(1,1)).toInt(),
	          2);

	EXPECT_EQ(m->data(m->index(1,2)).toInt(),
	          1);

	d_experiment.antShapes()->cloneShape(1,false,false);
	EXPECT_EQ(capsuleAdded.count(),6);
	d_experiment.antShapes()->cloneShape(1,false,true);
	EXPECT_EQ(capsuleAdded.count(),9);
	EXPECT_EQ(capsuleCleared.count(),1);

	d_experiment.antShapes()->clearCapsule(2);
	EXPECT_EQ(capsuleCleared.count(),2);

	EXPECT_EQ(m->data(m->index(1,1)).toInt(),
	          0);

	EXPECT_EQ(m->data(m->index(1,2)).toInt(),
	          0);

}
