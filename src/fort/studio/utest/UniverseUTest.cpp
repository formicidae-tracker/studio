#include "UniverseUTest.hpp"

#include <fort/studio/bridge/UniverseBridge.hpp>
#include <fort/studio/widget/UniverseEditorWidget.hpp>
#include "ui_UniverseEditorWidget.h"

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/Format.hpp>

#include <QSignalSpy>
#include <QTest>

fort::myrmidon::priv::TrackingDataDirectory::Ptr UniverseUTest::s_foo[3];


void UniverseUTest::SetUpTestSuite() {
	EXPECT_NO_THROW({
			s_foo[0] = fmp::priv::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",
			                                                  TestSetup::Basedir());
			s_foo[1] = fmp::priv::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",
			                                                  TestSetup::Basedir());
			s_foo[2] = fmp::priv::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002",
			                                                  TestSetup::Basedir());
		});
}

void UniverseUTest::SetUp() {
	EXPECT_NO_THROW({
			experiment = fmp::Experiment::Create(TestSetup::Basedir() / "universe.myrmidon");
			experiment->Save(TestSetup::Basedir() / "universe.myrmidon");
			auto foo = experiment->CreateSpace("foo");
			auto bar = experiment->CreateSpace("bar");
			experiment->AddTrackingDataDirectory(foo,s_foo[0]);
			experiment->AddTrackingDataDirectory(foo,s_foo[1]);
			experiment->AddTrackingDataDirectory(bar,s_foo[2]);
		});
	universe = new UniverseBridge(NULL);
}

void UniverseUTest::TearDown() {
	delete universe;
	experiment.reset();
}


TEST_F(UniverseUTest,Activation) {
	QSignalSpy activatedSignal(universe,SIGNAL(activated(bool)));
	EXPECT_FALSE(universe->isActive());
	EXPECT_EQ(activatedSignal.count(),0);

	universe->setExperiment(experiment);
	EXPECT_TRUE(universe->isActive());
	ASSERT_EQ(activatedSignal.count(),1);
	EXPECT_TRUE(activatedSignal.at(0).at(0).toBool());

	universe->setExperiment(experiment);
	EXPECT_TRUE(universe->isActive());
	ASSERT_EQ(activatedSignal.count(),2);
	EXPECT_TRUE(activatedSignal.at(1).at(0).toBool());

	universe->setExperiment(fmp::Experiment::Ptr());
	EXPECT_FALSE(universe->isActive());
	ASSERT_EQ(activatedSignal.count(),3);
	EXPECT_FALSE(activatedSignal.at(2).at(0).toBool());
}


TEST_F(UniverseUTest,AdditionAndDeletion) {
	QSignalSpy modifiedSignal(universe,SIGNAL(modified(bool)));
	QSignalSpy sapceAddedSignal(universe,SIGNAL(spaceAdded(const fmp::Space::Ptr &)));
	QSignalSpy spaceDeletedSignal(universe,SIGNAL(spaceDeleted(const QString &)));
	QSignalSpy spaceChangedSignal(universe,SIGNAL(spaceChanged(const fmp::Space::Ptr &)));
	QSignalSpy tddAdded(universe,SIGNAL(trackingDataDirectoryAdded(const fmp::TrackingDataDirectory::Ptr &)));
	QSignalSpy tddDeleted(universe,SIGNAL(trackingDataDirectoryDeleted(const QString &)));

	EXPECT_FALSE(universe->isModified());
	EXPECT_EQ(modifiedSignal.count(),0);

	universe->setExperiment(experiment);

	EXPECT_FALSE(universe->isModified());
	EXPECT_EQ(modifiedSignal.count(),0);


	auto m = universe->model();

	EXPECT_EQ(m->rowCount(),2);
	EXPECT_EQ(m->rowCount(m->index(0,0)),2);
	EXPECT_EQ(m->rowCount(m->index(1,0)),1);
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),"foo");
	EXPECT_EQ(ToStdString(m->data(m->index(1,0)).toString()),"bar");
	EXPECT_EQ(ToStdString(m->data(m->index(0,0,m->index(0,0))).toString()),"foo.0000");
	EXPECT_EQ(ToStdString(m->data(m->index(1,0,m->index(0,0))).toString()),"foo.0001");
	EXPECT_EQ(ToStdString(m->data(m->index(0,0,m->index(1,0))).toString()),"foo.0002");


	universe->addSpace("wuhu");
	EXPECT_TRUE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),1);
	EXPECT_TRUE(modifiedSignal.at(0).at(0).toBool());



	EXPECT_FALSE(universe->isDeletable({m->index(0,0)}));
	EXPECT_TRUE(universe->isDeletable({m->index(0,0,m->index(0,0))}));
	EXPECT_TRUE(universe->isDeletable({m->index(1,0,m->index(0,0))}));
	EXPECT_FALSE(universe->isDeletable({m->index(1,0)}));
	EXPECT_TRUE(universe->isDeletable({m->index(0,0,m->index(1,0))}));
	EXPECT_TRUE(universe->isDeletable({m->index(2,0)}));
	EXPECT_FALSE(universe->isDeletable({m->index(0,0),
	                                   m->index(0,0,m->index(0,0))}));
	EXPECT_TRUE(universe->isDeletable({m->index(0,0),
	                                   m->index(0,0,m->index(0,0)),
	                                   m->index(1,0,m->index(0,0))}));

	universe->setExperiment(experiment);
	EXPECT_FALSE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),2);
	EXPECT_FALSE(modifiedSignal.at(1).at(0).toBool());

	universe->deleteTrackingDataDirectory("foo.0001");
	EXPECT_TRUE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),3);
	EXPECT_TRUE(modifiedSignal.at(2).at(0).toBool());
	ASSERT_EQ(spaceChangedSignal.count(),1);
	ASSERT_EQ(tddDeleted.count(),1);
	EXPECT_EQ(tddDeleted.at(0).at(0).toString(),"foo.0001");
	EXPECT_EQ(spaceChangedSignal.at(0).at(0).value<fmp::Space::Ptr>()->URI(),
	          "spaces/1");

	universe->setExperiment(experiment);
	EXPECT_FALSE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),4);
	EXPECT_FALSE(modifiedSignal.at(3).at(0).toBool());

	universe->addTrackingDataDirectoryToSpace("wuhu",s_foo[1]);
	EXPECT_TRUE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),5);
	EXPECT_TRUE(modifiedSignal.at(4).at(0).toBool());
	ASSERT_EQ(spaceChangedSignal.count(),2);
	ASSERT_EQ(tddAdded.count(),1);
	EXPECT_EQ(tddAdded.at(0).at(0).value<fmp::TrackingDataDirectory::Ptr>()->URI(),"foo.0001");
	EXPECT_EQ(spaceChangedSignal.at(1).at(0).value<fmp::Space::Ptr>()->URI(),
	          "spaces/3");

	universe->setExperiment(experiment);
	EXPECT_FALSE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),6);
	EXPECT_FALSE(modifiedSignal.at(5).at(0).toBool());


	universe->deleteSelection({m->index(2,0),m->index(0,0,m->index(2,0))});
	EXPECT_TRUE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),7);
	EXPECT_TRUE(modifiedSignal.at(6).at(0).toBool());
	ASSERT_EQ(spaceDeletedSignal.count(),1);
	ASSERT_EQ(tddDeleted.count(),2);
	EXPECT_EQ(ToStdString(tddDeleted.at(1).at(0).toString()),"foo.0001");
	EXPECT_EQ(ToStdString(spaceDeletedSignal.at(0).at(0).toString()),
	          "wuhu");

	universe->setExperiment(experiment);
	EXPECT_FALSE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),8);
	EXPECT_FALSE(modifiedSignal.at(7).at(0).toBool());

	m->setData(m->index(0,0),"newName");
	EXPECT_TRUE(universe->isModified());
	ASSERT_EQ(modifiedSignal.count(),9);
	EXPECT_TRUE(modifiedSignal.at(8).at(0).toBool());
	ASSERT_EQ(spaceChangedSignal.count(),4);
	EXPECT_EQ(spaceChangedSignal.at(3).at(0).value<fmp::Space::Ptr>()->URI(),
	          "spaces/1");


}


TEST_F(UniverseUTest,WidgetTest) {
	UniverseEditorWidget widget;
	widget.setup(universe);

	QSignalSpy spaceDeleted(universe,SIGNAL(spaceDeleted(const QString &)));
	QSignalSpy tddDeleted(universe,SIGNAL(trackingDataDirectoryDeleted(const QString &)));


	auto ui = widget.d_ui;
	auto selection = ui->treeView->selectionModel();
	auto model = ui->treeView->model();
	EXPECT_FALSE(ui->addButton->isEnabled());
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	EXPECT_EQ(ui->treeView->model()->rowCount(),0);

	universe->setExperiment(experiment);
	EXPECT_TRUE(ui->addButton->isEnabled());
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	EXPECT_EQ(ui->treeView->model()->rowCount(),2);

	universe->addSpace("empty");

	selection->select(model->index(0,0),QItemSelectionModel::Select);
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	selection->select(model->index(0,0),QItemSelectionModel::Clear);
	selection->select(model->index(2,0),QItemSelectionModel::Select);
	EXPECT_TRUE(ui->deleteButton->isEnabled());
	selection->select(model->index(0,0),QItemSelectionModel::Clear);
	selection->select(model->index(0,0,model->index(1,0)),QItemSelectionModel::Select);
	ASSERT_TRUE(ui->deleteButton->isEnabled());
	QTest::mouseClick(ui->deleteButton,Qt::LeftButton);
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	ASSERT_EQ(tddDeleted.count(),1);
	EXPECT_EQ(tddDeleted.at(0).at(0),"foo.0002");
	selection->select(model->index(2,0),QItemSelectionModel::Select);
	ASSERT_TRUE(ui->deleteButton->isEnabled());
	QTest::mouseClick(ui->deleteButton,Qt::LeftButton);
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	ASSERT_EQ(spaceDeleted.count(),1);
	EXPECT_EQ(spaceDeleted.at(0).at(0),"empty");

	selection->select(model->index(0,0,model->index(0,0)),QItemSelectionModel::Select);
	selection->select(model->index(1,0,model->index(0,0)),QItemSelectionModel::Select);
	selection->select(model->index(0,0),QItemSelectionModel::Select);
	ASSERT_TRUE(ui->deleteButton->isEnabled());
	QTest::mouseClick(ui->deleteButton,Qt::LeftButton);
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	ASSERT_EQ(spaceDeleted.count(),2);
	EXPECT_EQ(spaceDeleted.at(1).at(0),"foo");
	ASSERT_EQ(tddDeleted.count(),3);
	EXPECT_EQ(tddDeleted.at(1).at(0),"foo.0000");
	EXPECT_EQ(tddDeleted.at(2).at(0),"foo.0001");
}
