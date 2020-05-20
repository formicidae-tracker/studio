#include "ZoneUTest.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include <QSignalSpy>
#include <QAbstractItemModel>

#include <fort/studio/Format.hpp>

void ZoneUTest::SetUp() {
	EXPECT_NO_THROW({
			experiment = fmp::Experiment::Create(TestSetup::Basedir() / "zone.myrmidon");
			experiment->Save(TestSetup::Basedir() / "zone.myrmidon");
			auto foo = experiment->CreateSpace("foo");
			auto bar = experiment->CreateSpace("bar");
		});

	zones = new ZoneBridge(NULL);

}

void ZoneUTest::TearDown() {
	experiment.reset();
	delete zones;
}


TEST_F(ZoneUTest,Activation) {
	QSignalSpy activated(zones,SIGNAL(activated(bool)));
	EXPECT_FALSE(zones->isActive());
	EXPECT_EQ(activated.count(),0);

	zones->setExperiment(experiment);
	EXPECT_TRUE(zones->isActive());
	ASSERT_EQ(activated.count(),1);
	EXPECT_TRUE(activated.last().at(0).toBool());

	zones->setExperiment(experiment);
	EXPECT_TRUE(zones->isActive());
	EXPECT_EQ(activated.count(),2);
	EXPECT_TRUE(activated.last().at(0).toBool());

	zones->setExperiment(fmp::Experiment::Ptr());
	EXPECT_FALSE(zones->isActive());
	EXPECT_EQ(activated.count(),3);
	EXPECT_FALSE(activated.last().at(0).toBool());

	zones->setExperiment(fmp::Experiment::Ptr());
	EXPECT_FALSE(zones->isActive());
	EXPECT_EQ(activated.count(),4);
	EXPECT_FALSE(activated.last().at(0).toBool());
}


TEST_F(ZoneUTest,ZoneManipulation) {
	QSignalSpy modified(zones,SIGNAL(modified(bool)));

	zones->setExperiment(experiment);
	EXPECT_FALSE(zones->isModified());
	EXPECT_EQ(modified.count(),0);

	auto m = zones->spaceModel();
	ASSERT_EQ(m->rowCount(),2);
	EXPECT_EQ(m->rowCount(m->index(0,0)),0);
	EXPECT_EQ(m->rowCount(m->index(1,0)),0);

	EXPECT_EQ(m->data(m->index(0,2),Qt::DisplayRole).toInt(),0);
	EXPECT_EQ(m->data(m->index(1,2),Qt::DisplayRole).toInt(),0);



	//will add a zone
	EXPECT_TRUE(zones->canAddItemAt(m->index(0,1)));
	zones->addItemAtIndex(m->index(0,1));
	EXPECT_TRUE(zones->isModified());
	ASSERT_EQ(modified.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(m->rowCount(m->index(0,0)),1);
	EXPECT_EQ(m->rowCount(m->index(0,0,m->index(0,0))),1);
	EXPECT_EQ(m->data(m->index(0,2),Qt::DisplayRole).toInt(),1);
	EXPECT_EQ(m->data(m->index(0,2,m->index(0,0)),Qt::DisplayRole).toInt(),1);



	zones->setExperiment(experiment);
	EXPECT_FALSE(zones->isModified());
	EXPECT_EQ(modified.count(),2);
	EXPECT_FALSE(modified.last().at(0).toBool());

	auto firstSpaceIndex= m->index(0,0);
	auto firstZoneIndex = m->index(0,0,firstSpaceIndex);


	EXPECT_TRUE( (m->flags(m->index(0,1,firstSpaceIndex)) & Qt::ItemIsEditable) != 0);
	EXPECT_TRUE( (m->flags(m->index(0,1,firstZoneIndex)) & Qt::ItemIsEditable) != 0);
	EXPECT_TRUE( (m->flags(m->index(0,0,firstZoneIndex)) & Qt::ItemIsEditable) != 0);
	EXPECT_FALSE(zones->canAddItemAt(m->index(0,0,firstZoneIndex)));
	m->setData(m->index(0,1,firstZoneIndex),
	           ToQString(fm::Time::FromTimeT(1)));

	EXPECT_TRUE(zones->canAddItemAt(m->index(0,0,firstZoneIndex)));
	EXPECT_TRUE(zones->isModified());
	EXPECT_EQ(modified.count(),3);
	EXPECT_TRUE(modified.last().at(0).toBool());

	zones->setExperiment(experiment);
	EXPECT_FALSE(zones->isModified());
	EXPECT_EQ(modified.count(),4);
	EXPECT_FALSE(modified.last().at(0).toBool());
	firstSpaceIndex= m->index(0,0);
	firstZoneIndex = m->index(0,0,firstSpaceIndex);


	zones->addItemAtIndex(m->index(0,0,firstZoneIndex));
	EXPECT_TRUE(zones->isModified());
	EXPECT_EQ(modified.count(),5);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(m->rowCount(firstZoneIndex),2);
	EXPECT_EQ(m->data(m->index(0,2,firstSpaceIndex),Qt::DisplayRole).toInt(),2);

	zones->setExperiment(experiment);
	EXPECT_FALSE(zones->isModified());
	EXPECT_EQ(modified.count(),6);
	EXPECT_FALSE(modified.last().at(0).toBool());
	firstSpaceIndex= m->index(0,0);
	firstZoneIndex = m->index(0,0,firstSpaceIndex);

	EXPECT_FALSE(zones->canRemoveItemAt(firstSpaceIndex));
	EXPECT_TRUE(zones->canRemoveItemAt(firstZoneIndex));
	EXPECT_TRUE(zones->canRemoveItemAt(m->index(0,0,firstZoneIndex)));
	EXPECT_TRUE(zones->canRemoveItemAt(m->index(1,0,firstZoneIndex)));

	zones->removeItemAtIndex(m->index(0,0,firstZoneIndex));
	EXPECT_TRUE(zones->isModified());
	EXPECT_EQ(modified.count(),7);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(m->rowCount(firstZoneIndex),1);
	EXPECT_EQ(m->data(m->index(0,2,firstSpaceIndex),Qt::DisplayRole).toInt(),1);

	zones->setExperiment(experiment);
	EXPECT_FALSE(zones->isModified());
	EXPECT_EQ(modified.count(),8);
	EXPECT_FALSE(modified.last().at(0).toBool());
	firstSpaceIndex= m->index(0,0);
	firstZoneIndex = m->index(0,0,firstSpaceIndex);

	zones->removeItemAtIndex(m->index(0,0,firstSpaceIndex));
	EXPECT_TRUE(zones->isModified());
	EXPECT_EQ(modified.count(),9);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(m->rowCount(firstSpaceIndex),0);
	EXPECT_EQ(m->data(m->index(0,2),Qt::DisplayRole).toInt(),0);

}
