#include "UniverseUTest.hpp"

#include "UniverseBridge.hpp"

#include <myrmidon/TestSetup.hpp>

#include <QSignalSpy>

TEST_F(UniverseUTest,TestModificationActivation) {

	fmp::Experiment::Ptr experiment;
	UniverseBridge universe(NULL);
	fmp::TrackingDataDirectory::ConstPtr foo[3];
	ASSERT_NO_THROW({
			experiment = fmp::priv::Experiment::NewFile(TestSetup::Basedir() / "universe.myrmidon");
			foo[0] = fmp::priv::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",
			                                                TestSetup::Basedir());
			foo[1] = fmp::priv::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",
			                                                TestSetup::Basedir());
			foo[2] = fmp::priv::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0002",
			                                                TestSetup::Basedir());
			auto bar = experiment->CreateSpace("bar");
			auto baz = experiment->CreateSpace("baz");

			bar->AddTrackingDataDirectory(foo[0]);
			bar->AddTrackingDataDirectory(foo[1]);
			baz->AddTrackingDataDirectory(foo[2]);
		});

	QSignalSpy activatedSignal(&universe,SIGNAL(activated(bool)));
	QSignalSpy modifiedSignal(&universe,SIGNAL(modified(bool)));
	QSignalSpy sapceAddedSignal(&universe,SIGNAL(spaceAdded(const fmp::Space::Ptr &)));
	QSignalSpy spaceDeletedSignal(&universe,SIGNAL(spaceDeleted(const QString &)));
	QSignalSpy spaceChangedSignal(&universe,SIGNAL(spaceChanged(const fmp::Space::Ptr &)));
	QSignalSpy tddAdded(&universe,SIGNAL(trackingDataDirectoryAdded(const fmp::TrackingDataDirectory::ConstPtr &)));
	QSignalSpy tddDeleted(&universe,SIGNAL(trackingDataDirectoryDeleted(const QString &)));

	EXPECT_FALSE(universe.isActive());
	EXPECT_FALSE(universe.isModified());
	EXPECT_EQ(activatedSignal.count(),0);
	EXPECT_EQ(modifiedSignal.count(),0);

	universe.setExperiment(experiment);

	EXPECT_TRUE(universe.isActive());
	EXPECT_FALSE(universe.isModified());
	ASSERT_EQ(activatedSignal.count(),1);
	EXPECT_TRUE(activatedSignal.at(0).at(0).toBool());

	auto m = universe.model();

	EXPECT_EQ(m->rowCount(),2);
	EXPECT_EQ(m->rowCount(m->index(0,0)),2);
	EXPECT_EQ(m->rowCount(m->index(1,0)),1);
	EXPECT_EQ(m->data(m->index(0,0)).toString(),"bar");
	EXPECT_EQ(m->data(m->index(1,0)).toString(),"baz");
	EXPECT_EQ(m->data(m->index(0,0,m->index(0,0))).toString(),"foo.0000");
	EXPECT_EQ(m->data(m->index(1,0,m->index(0,0))).toString(),"foo.0001");
	EXPECT_EQ(m->data(m->index(0,0,m->index(1,0))).toString(),"foo.0002");


	universe.addSpace("wuhu");
	EXPECT_TRUE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),1);
	EXPECT_TRUE(modifiedSignal.at(0).at(0).toBool());



	EXPECT_FALSE(universe.isDeletable(m->index(0,0)));
	EXPECT_TRUE(universe.isDeletable(m->index(0,0,m->index(0,0))));
	EXPECT_TRUE(universe.isDeletable(m->index(1,0,m->index(0,0))));
	EXPECT_FALSE(universe.isDeletable(m->index(1,0)));
	EXPECT_TRUE(universe.isDeletable(m->index(0,0,m->index(1,0))));
	EXPECT_TRUE(universe.isDeletable(m->index(2,0)));

	universe.setExperiment(experiment);
	EXPECT_FALSE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),2);
	EXPECT_FALSE(modifiedSignal.at(1).at(0).toBool());

	universe.deleteTrackingDataDirectory("foo.0001");
	EXPECT_TRUE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),3);
	EXPECT_TRUE(modifiedSignal.at(2).at(0).toBool());
	ASSERT_EQ(spaceChangedSignal.count(),1);
	ASSERT_EQ(tddDeleted.count(),1);
	EXPECT_EQ(tddDeleted.at(0).at(0).toString(),"foo.0001");
	EXPECT_EQ(spaceChangedSignal.at(0).at(0).value<fmp::Space::Ptr>()->URI().string(),
	          "bar");

	universe.setExperiment(experiment);
	EXPECT_FALSE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),4);
	EXPECT_FALSE(modifiedSignal.at(3).at(0).toBool());

	universe.addTrackingDataDirectoryToSpace("wuhu",foo[1]);
	EXPECT_TRUE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),5);
	EXPECT_TRUE(modifiedSignal.at(4).at(0).toBool());
	ASSERT_EQ(spaceChangedSignal.count(),2);
	ASSERT_EQ(tddAdded.count(),1);
	EXPECT_EQ(tddAdded.at(0).at(0).value<fmp::TrackingDataDirectory::ConstPtr>()->URI().string(),"foo.0001");
	EXPECT_EQ(spaceChangedSignal.at(1).at(0).value<fmp::Space::Ptr>()->URI().string(),
	          "wuhu");

	universe.setExperiment(experiment);
	EXPECT_FALSE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),6);
	EXPECT_FALSE(modifiedSignal.at(5).at(0).toBool());


	universe.deleteSelection({m->index(2,0),m->index(0,0,m->index(2,0))});
	EXPECT_TRUE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),7);
	EXPECT_TRUE(modifiedSignal.at(6).at(0).toBool());
	ASSERT_EQ(spaceDeletedSignal.count(),1);
	ASSERT_EQ(tddDeleted.count(),2);
	EXPECT_EQ(tddDeleted.at(1).at(0).toString(),"foo.0001");
	EXPECT_EQ(spaceDeletedSignal.at(0).at(0).toString(),
	          "wuhu");

	universe.setExperiment(experiment);
	EXPECT_FALSE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),8);
	EXPECT_FALSE(modifiedSignal.at(7).at(0).toBool());

	m->setData(m->index(0,0),"newName");
	EXPECT_TRUE(universe.isModified());
	ASSERT_EQ(modifiedSignal.count(),9);
	EXPECT_TRUE(modifiedSignal.at(8).at(0).toBool());
	ASSERT_EQ(spaceChangedSignal.count(),4);
	EXPECT_EQ(spaceChangedSignal.at(3).at(0).value<fmp::Space::Ptr>()->URI().string(),
	          "newName");



}
