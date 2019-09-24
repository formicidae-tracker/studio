#include "TrackingDataDirectoryUTest.hpp"


#include "TrackingDataDirectory.hpp"

#include "../TestSetup.hpp"

namespace fmp = fort::myrmidon::priv;

TEST_F(TrackingDataDirectoryUTest,ExtractInfoFromTrackingDatadirectories) {
	try {
		auto tdd = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",TestSetup::Basedir());
		ASSERT_EQ(tdd.Path,"foo.0001");
		ASSERT_EQ(tdd.StartFrame,5);
		ASSERT_EQ(tdd.EndFrame,8);

	} catch( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

	EXPECT_THROW({
			//no tracking data
			auto tdd = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir());
		}, std::invalid_argument);

	EXPECT_THROW({
			//directory does not exists
			auto tdd = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.1234",TestSetup::Basedir());
		}, std::invalid_argument);


	EXPECT_THROW({
			// is not a directory
			auto tdd = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "test.myrmidon",TestSetup::Basedir());
		}, std::invalid_argument);
}
