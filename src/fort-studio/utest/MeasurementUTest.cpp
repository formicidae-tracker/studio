#include "MeasurementUTest.hpp"

#include <fort-studio/widget/MeasurementTypeWidget.hpp>
#include "ui_MeasurementTypeWidget.h"

#include <myrmidon/TestSetup.hpp>

#include <QTest>
#include <QSignalSpy>


void MeasurementUTest::SetUp() {
	experiment = fmp::Experiment::NewFile(TestSetup::Basedir() / "measurementUTest.myrmidon");
	measurements = new MeasurementBridge(NULL);
}
void MeasurementUTest::TearDown() {
	delete measurements;
	experiment.reset();
}


TEST_F(MeasurementUTest,BridgeActivation) {
	QSignalSpy activated(measurements,SIGNAL(activated(bool)));
	EXPECT_FALSE(measurements->isActive());
	EXPECT_EQ(activated.count(),0);

	measurements->setExperiment(experiment);
	EXPECT_TRUE(measurements->isActive());
	ASSERT_EQ(activated.count(),1);
	EXPECT_TRUE(activated.at(0).at(0).toBool());

	measurements->setExperiment(experiment);
	EXPECT_TRUE(measurements->isActive());
	ASSERT_EQ(activated.count(),2);
	EXPECT_TRUE(activated.at(1).at(0).toBool());


	measurements->setExperiment(fmp::Experiment::Ptr());
	EXPECT_FALSE(measurements->isActive());
	ASSERT_EQ(activated.count(),3);
	EXPECT_FALSE(activated.at(2).at(0).toBool());

}

TEST_F(MeasurementUTest,TypeBridgeModification) {
	QSignalSpy modified(measurements,SIGNAL(modified(bool)));
	QSignalSpy typeModified(measurements,SIGNAL(measurementTypeModified(quint32,QString)));
	QSignalSpy deleted(measurements,SIGNAL(measurementTypeDeleted(quint32)));
	EXPECT_FALSE(measurements->isModified());
	EXPECT_EQ(modified.count(),0);

	measurements->setExperiment(experiment);
	auto m = measurements->measurementTypeModel();

	EXPECT_FALSE(measurements->isModified());
	ASSERT_EQ(modified.count(),0);
	EXPECT_EQ(m->rowCount(),1);


	measurements->setMeasurementType(-1,"foo");
	EXPECT_TRUE(measurements->isModified());
	ASSERT_EQ(modified.count(),1);
	ASSERT_EQ(typeModified.count(),1);
	EXPECT_TRUE(modified.at(0).at(0).toBool());
	ASSERT_EQ(m->rowCount(),2);
	EXPECT_EQ(typeModified.last().at(0).toInt(),1);
	EXPECT_EQ(typeModified.last().at(1).toString(),"foo");

	measurements->setExperiment(experiment);
	EXPECT_FALSE(modified.last().at(0).toBool());
	measurements->setMeasurementType(1,"bar");
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(typeModified.last().at(0).toInt(),1);
	EXPECT_EQ(typeModified.last().at(1).toString(),"bar");


	measurements->setExperiment(experiment);
	EXPECT_FALSE(modified.last().at(0).toBool());
	measurements->deleteMeasurementType(1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	ASSERT_EQ(deleted.count(),1);
	EXPECT_EQ(deleted.last().at(0).toInt(),1);

}


TEST_F(MeasurementUTest,TypeWidgetTest) {
	MeasurementTypeWidget widget(NULL);
	widget.setup(measurements);
	QSignalSpy typeModified(measurements,SIGNAL(measurementTypeModified(quint32,QString)));
	QSignalSpy deleted(measurements,SIGNAL(measurementTypeDeleted(quint32)));

	auto ui = widget.d_ui;
	auto m = measurements->measurementTypeModel();
	EXPECT_FALSE(ui->addButton->isEnabled());
	EXPECT_FALSE(ui->deleteButton->isEnabled());

	measurements->setExperiment(experiment);
	EXPECT_TRUE(ui->addButton->isEnabled());
	EXPECT_FALSE(ui->deleteButton->isEnabled());

	QTest::mouseClick(ui->addButton,Qt::LeftButton);
	EXPECT_EQ(m->rowCount(),2);
	EXPECT_EQ(std::string(m->data(m->index(1,1)).toString().toUtf8().constData()),"new-measurement-type");
	EXPECT_EQ(m->data(m->index(1,0)).toInt(),1);
	m->setData(m->index(1,1),"foobar");
	ASSERT_FALSE(typeModified.isEmpty());
	EXPECT_EQ(typeModified.last().at(0).toInt(),1);
	EXPECT_EQ(typeModified.last().at(1).toString(),"foobar");

	auto selection = ui->tableView->selectionModel();
	selection->select(m->index(0,0),QItemSelectionModel::Select);
	selection->select(m->index(0,1),QItemSelectionModel::Select);
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	selection->select(m->index(0,0),QItemSelectionModel::Clear);
	selection->select(m->index(1,0),QItemSelectionModel::Select);
	selection->select(m->index(1,1),QItemSelectionModel::Select);
	EXPECT_TRUE(ui->deleteButton->isEnabled());

	QTest::mouseClick(ui->deleteButton,Qt::LeftButton);
	ASSERT_FALSE(deleted.isEmpty());
	EXPECT_EQ(deleted.last().at(0).toInt(),1);
}
