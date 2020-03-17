#include "AntShapeTypeUTest.hpp"

#include <fort-studio/bridge/AntShapeTypeBridge.hpp>
#include <fort-studio/widget/AntShapeTypeEditorWidget.hpp>
#include "ui_AntShapeTypeEditorWidget.h"

#include <fort-studio/Format.hpp>

#include <myrmidon/TestSetup.hpp>

#include <QSignalSpy>
#include <QTest>
#include <QAbstractItemModel>

void AntShapeTypeUTest::SetUp() {
	experiment = fmp::Experiment::Create(TestSetup::Basedir() / "ant-shape-types.myrmidon");
	shapeTypes = new AntShapeTypeBridge(nullptr);
}

void AntShapeTypeUTest::TearDown() {
	delete shapeTypes;
	experiment.reset();
}


TEST_F(AntShapeTypeUTest,Activation) {
	QSignalSpy activated(shapeTypes,SIGNAL(activated(bool)));

	EXPECT_FALSE(shapeTypes->isActive());
	EXPECT_EQ(activated.count(),0);

	shapeTypes->setExperiment(experiment);
	EXPECT_TRUE(shapeTypes->isActive());
	ASSERT_EQ(activated.count(),1);
	EXPECT_TRUE(activated.last().at(0).toBool());

	shapeTypes->setExperiment(experiment);
	EXPECT_TRUE(shapeTypes->isActive());
	EXPECT_EQ(activated.count(),2);
	EXPECT_TRUE(activated.last().at(0).toBool());

	shapeTypes->setExperiment(fmp::Experiment::Ptr());
	EXPECT_FALSE(shapeTypes->isActive());
	EXPECT_EQ(activated.count(),3);
	EXPECT_FALSE(activated.last().at(0).toBool());

}


TEST_F(AntShapeTypeUTest,ModificationTest) {
	shapeTypes->setExperiment(experiment);

	QSignalSpy modified(shapeTypes,SIGNAL(modified(bool)));
	QSignalSpy typeModified(shapeTypes,SIGNAL(typeModified(quint32,QString)));
	QSignalSpy typeDeleted(shapeTypes,SIGNAL(typeDeleted(quint32)));
	auto m = shapeTypes->shapeModel();

	EXPECT_EQ(m->rowCount(),0);
	EXPECT_FALSE(shapeTypes->isModified());
	EXPECT_EQ(modified.count(),0);

	shapeTypes->addType("foo");
	EXPECT_EQ(m->rowCount(),1);
	ASSERT_EQ(modified.count(),1);
	ASSERT_EQ(typeModified.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(ToStdString(typeModified.last().at(1).toString()),
	          "foo");
	EXPECT_EQ(typeModified.last().at(0).toInt(),
	          1);

	shapeTypes->setExperiment(experiment);
	EXPECT_FALSE(shapeTypes->isModified());
	EXPECT_EQ(modified.count(),2);
	EXPECT_FALSE(modified.last().at(0).toBool());

	m->setData(m->index(0,0),"bar");
	EXPECT_TRUE(shapeTypes->isModified());
	EXPECT_EQ(modified.count(),3);
	EXPECT_EQ(typeModified.count(),2);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(typeModified.last().at(0).toInt(),1);
	EXPECT_EQ(ToStdString(typeModified.last().at(1).toString()),
	          "bar");

	shapeTypes->setExperiment(experiment);
	EXPECT_FALSE(shapeTypes->isModified());
	EXPECT_EQ(modified.count(),4);
	EXPECT_FALSE(modified.last().at(0).toBool());

	shapeTypes->deleteType(1);
	EXPECT_TRUE(shapeTypes->isModified());
	EXPECT_EQ(modified.count(),5);
	ASSERT_EQ(typeDeleted.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(typeDeleted.last().at(0).toInt(),1);

}


TEST_F(AntShapeTypeUTest,AntShapeTypeEditorWidgetTest) {
	AntShapeTypeEditorWidget widget(NULL);
	widget.setup(shapeTypes);
	auto ui = widget.d_ui;

	QSignalSpy typeModified(shapeTypes,SIGNAL(typeModified(quint32,QString)));
	QSignalSpy typeDeleted(shapeTypes,SIGNAL(typeDeleted(quint32)));

	EXPECT_FALSE(ui->addButton->isEnabled());
	EXPECT_FALSE(ui->removeButton->isEnabled());

	shapeTypes->setExperiment(experiment);
	EXPECT_TRUE(ui->addButton->isEnabled());
	EXPECT_FALSE(ui->removeButton->isEnabled());

	QTest::mouseClick(ui->addButton,Qt::LeftButton);
	ASSERT_EQ(typeModified.count(),1);
	EXPECT_EQ(typeModified.last().at(0).toInt(),1);
	EXPECT_EQ(ToStdString(typeModified.last().at(1).toString()),
	          "body part 1");

	QTest::mouseClick(ui->addButton,Qt::LeftButton);
	ASSERT_EQ(typeModified.count(),2);
	EXPECT_EQ(typeModified.last().at(0).toInt(),2);
	EXPECT_EQ(ToStdString(typeModified.last().at(1).toString()),
	          "body part 2");

	int xPos = ui->tableView->columnViewportPosition( 1 );
	int yPos = ui->tableView->rowViewportPosition( 0 );

	QTest::mouseDClick(ui->tableView->viewport(), Qt::LeftButton, NULL, QPoint( xPos, yPos ));
	EXPECT_TRUE(ui->removeButton->isEnabled());
	QTest::mouseClick(ui->removeButton, Qt::LeftButton);
	ASSERT_EQ(typeDeleted.count(),1);
	EXPECT_EQ(typeDeleted.last().at(0).toInt(),1);

	auto m = shapeTypes->shapeModel();
	EXPECT_EQ(ToStdString(m->data(m->index(0,0),Qt::DisplayRole).toString()),
	          "body part 2");
	EXPECT_EQ(ToStdString(m->data(m->index(0,1),Qt::DisplayRole).toString()),
	          "2");

}
