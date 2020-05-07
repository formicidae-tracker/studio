#include "ExperimentBridgeUTest.hpp"

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/studio/bridge/ExperimentBridge.hpp>

#include <QSignalSpy>

void ExperimentBridgeUTest::SetUp() {
	try {
		pathExisting = TestSetup::Basedir() / "ExperimentBridge.myrmidon";
		auto experiment = fmp::Experiment::NewFile(pathExisting);
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Could not create experiment file: " << e.what();
	}

}


TEST_F(ExperimentBridgeUTest,ActiveModifiedState) {
	auto pathCreated = TestSetup::Basedir() / "ExperimentBridge.2.myrmidon";
	ExperimentBridge controller;
	QSignalSpy modifiedSpy(&controller,SIGNAL(modified(bool)));
	QSignalSpy activatedSpy(&controller,SIGNAL(activated(bool)));

	EXPECT_EQ(controller.isActive(),false);
	EXPECT_EQ(controller.isModified(),false);
	EXPECT_EQ(modifiedSpy.count(),0);
	EXPECT_EQ(activatedSpy.count(),0);

	ASSERT_TRUE(controller.open(pathExisting.c_str()));
	EXPECT_EQ(controller.absoluteFilePath().generic_string(),
	          pathExisting.generic_string());
	EXPECT_EQ(controller.isModified(),false);
	EXPECT_EQ(controller.isActive(),true);
	EXPECT_EQ(modifiedSpy.count(),0);
	ASSERT_EQ(activatedSpy.count(),1);
	EXPECT_EQ(activatedSpy.at(0).at(0).toBool(),true);

	ASSERT_TRUE(controller.create(pathCreated.c_str()));
	EXPECT_EQ(controller.absoluteFilePath().generic_string(),
	          pathCreated.generic_string());
	EXPECT_EQ(controller.isModified(),false);
	EXPECT_EQ(controller.isActive(),true);
	EXPECT_EQ(modifiedSpy.count(),0);
	ASSERT_EQ(activatedSpy.count(),2);
	EXPECT_EQ(activatedSpy.at(1).at(0).toBool(),true);


	controller.setModified(true);
	ASSERT_EQ(modifiedSpy.count(),1);
	EXPECT_EQ(modifiedSpy.at(0).at(0).toBool(),true);
	controller.setModified(false);
	ASSERT_EQ(modifiedSpy.count(),2);
	EXPECT_EQ(modifiedSpy.at(1).at(0).toBool(),false);

	// test child connections
	std::vector<Bridge*> childs =
		{
		 controller.universe(),
		 controller.measurements(),
		 controller.identifier(),
		 controller.globalProperties(),
		 controller.selectedAnt(),
		 controller.selectedIdentification(),
		 controller.antShapeTypes(),
		 controller.antMetadata(),
		};
	size_t expected = 2;
	for ( const auto & b : childs ) {

		b->setModified(true);

		++expected;
		ASSERT_EQ(modifiedSpy.count(),expected);
		EXPECT_EQ(modifiedSpy.at(expected-1).at(0).toBool(),true);

		controller.setModified(false);

		++expected;
		ASSERT_EQ(modifiedSpy.count(),expected);
		EXPECT_EQ(modifiedSpy.at(expected-1).at(0).toBool(),false);

	}

}

TEST_F(ExperimentBridgeUTest,TDDCloseUpDetectionIsOrderSafe) {

	// addresses bug #61, setting TDD first and then Tag family should not matter
	ExperimentBridge experiment;
	ASSERT_TRUE(experiment.open(pathExisting.c_str()));
	auto globalProperties = experiment.globalProperties();
	auto universe = experiment.universe();

	auto tdd = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000", TestSetup::Basedir());

	EXPECT_NO_THROW({
			globalProperties->setTagFamily(fort::tags::Family::Tag36h11);
			universe->addSpace("foo");
			universe->addTrackingDataDirectoryToSpace("foo",tdd);
		});

	ASSERT_TRUE(experiment.open(pathExisting.c_str()));

	//sets tdd first and then the family from undefined
	EXPECT_NO_THROW({
			universe->addSpace("foo");
			universe->addTrackingDataDirectoryToSpace("foo",tdd);
			globalProperties->setTagFamily(fort::tags::Family::Tag36h11);
		});
}
