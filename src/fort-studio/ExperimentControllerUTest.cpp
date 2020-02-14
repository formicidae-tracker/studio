#include "ExperimentControllerUTest.hpp"

#include <myrmidon/TestSetup.hpp>
#include "ExperimentController.hpp"

#include <QSignalSpy>

fmp::Experiment::Ptr ExperimentControllerUTest::s_experiment;

void ExperimentControllerUTest::SetUpTestSuite() {
	EXPECT_NO_THROW({
			s_experiment = fmp::Experiment::NewFile(TestSetup::Basedir() / "ExperimentController.myrmidon");
			s_experiment->Save(s_experiment->AbsoluteFilePath());
		});
}
void ExperimentControllerUTest::TearDownTestSuite() {
	s_experiment.reset();
}


TEST_F(ExperimentControllerUTest,OpeningActiveModifiedState) {
	auto pathExisting = TestSetup::Basedir() / "ExperimentController.myrmidon";
	auto pathCreated = TestSetup::Basedir() / "ExperimentController.2.myrmidon";
	auto  controller = new ExperimentController();
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

}
