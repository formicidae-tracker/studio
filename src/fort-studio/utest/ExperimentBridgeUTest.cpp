#include "ExperimentBridgeUTest.hpp"

#include <myrmidon/TestSetup.hpp>
#include <fort-studio/bridge/ExperimentBridge.hpp>

#include <QSignalSpy>

fmp::Experiment::Ptr ExperimentBridgeUTest::s_experiment;

void ExperimentBridgeUTest::SetUpTestSuite() {
	EXPECT_NO_THROW({
			s_experiment = fmp::Experiment::NewFile(TestSetup::Basedir() / "ExperimentBridge.myrmidon");
			s_experiment->Save(s_experiment->AbsoluteFilePath());
		});
}
void ExperimentBridgeUTest::TearDownTestSuite() {
	s_experiment.reset();
}


TEST_F(ExperimentBridgeUTest,ActiveModifiedState) {
	auto pathExisting = TestSetup::Basedir() / "ExperimentBridge.myrmidon";
	auto pathCreated = TestSetup::Basedir() / "ExperimentBridge.2.myrmidon";
	auto  controller = new ExperimentBridge();
	QSignalSpy modifiedSpy(controller,SIGNAL(modified(bool)));
	QSignalSpy activatedSpy(controller,SIGNAL(activated(bool)));

	EXPECT_EQ(controller->isActive(),false);
	EXPECT_EQ(controller->isModified(),false);
	EXPECT_EQ(modifiedSpy.count(),0);
	EXPECT_EQ(activatedSpy.count(),0);

	ASSERT_TRUE(controller->open(pathExisting.c_str()));
	EXPECT_EQ(controller->absoluteFilePath().generic_string(),
	          pathExisting.generic_string());
	EXPECT_EQ(controller->isModified(),false);
	EXPECT_EQ(controller->isActive(),true);
	EXPECT_EQ(modifiedSpy.count(),0);
	ASSERT_EQ(activatedSpy.count(),1);
	EXPECT_EQ(activatedSpy.at(0).at(0).toBool(),true);

	ASSERT_TRUE(controller->create(pathCreated.c_str()));
	EXPECT_EQ(controller->absoluteFilePath().generic_string(),
	          pathCreated.generic_string());
	EXPECT_EQ(controller->isModified(),false);
	EXPECT_EQ(controller->isActive(),true);
	EXPECT_EQ(modifiedSpy.count(),0);
	ASSERT_EQ(activatedSpy.count(),2);
	EXPECT_EQ(activatedSpy.at(1).at(0).toBool(),true);


	controller->setModified(true);
	ASSERT_EQ(modifiedSpy.count(),1);
	EXPECT_EQ(modifiedSpy.at(0).at(0).toBool(),true);
	controller->setModified(false);
	ASSERT_EQ(modifiedSpy.count(),2);
	EXPECT_EQ(modifiedSpy.at(1).at(0).toBool(),false);

	// test child connections
	std::vector<Bridge*> childs =
		{
		 controller->universe(),
		 controller->measurements(),
		 controller->identifier(),
		 controller->globalProperties(),
		 controller->selectedAnt(),
		 controller->selectedIdentification(),
		};
	size_t expected = 2;
	for ( const auto & b : childs ) {

		b->setModified(true);

		++expected;
		ASSERT_EQ(modifiedSpy.count(),expected);
		EXPECT_EQ(modifiedSpy.at(expected-1).at(0).toBool(),true);

		controller->setModified(false);

		++expected;
		ASSERT_EQ(modifiedSpy.count(),expected);
		EXPECT_EQ(modifiedSpy.at(expected-1).at(0).toBool(),false);

	}

}
