#include "TagCloseUpBridge.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/UniverseBridge.hpp>
#include <fort/studio/bridge/TagCloseUpBridge.hpp>

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>


void TagCloseUpUTest::SetUp() {
	experiment = fmp::Experiment::Create(TestSetup::Basedir() / "tag-close-up.myrmidon");
	bridge = new ExperimentBridge();
	bridge->setExperiment(experiment);
	bridge->universe()->addSpace("foo");
}

void TagCloseUpUTest::TearDown() {
	delete bridge;
}


TEST_F(TagCloseUpUTest,ListTagsForNewFolder) {
	auto model = bridge->tagCloseUps()->tagModel();

	EXPECT_EQ(model->rowCount(),0);


	auto foo = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir());
	auto loaders = foo->PrepareTagCloseUpsLoaders();
	for ( const auto & l : loaders ) {
		l();
	}
	bridge->universe()->addTrackingDataDirectoryToSpace("foo",foo);

	ASSERT_EQ(model->rowCount(),1);
	EXPECT_EQ(model->data(model->index(0,1)).toInt(),1);

}
