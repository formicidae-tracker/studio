#include "TrackingDataDirectoryUTest.hpp"


#include "TrackingDataDirectory.hpp"
#include <myrmidon/Experiment.pb.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/message_differencer.h>

#include "../TestSetup.hpp"

namespace fmp = fort::myrmidon::priv;

TEST_F(TrackingDataDirectoryUTest,ExtractInfoFromTrackingDatadirectories) {
	try {
		auto tdd = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001",TestSetup::Basedir());
		ASSERT_EQ(tdd.Path(),"foo.0001");
		ASSERT_EQ(tdd.StartFrame(),5);
		ASSERT_EQ(tdd.EndFrame(),8);

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


TEST_F(TrackingDataDirectoryUTest,HasUIDBasedOnPath) {
	struct TestData {
		std::pair<fs::path,fs::path> A,B;
		bool Expected;
	};

	std::vector<TestData> data
		= {
		   {
		    std::make_pair("foo","bar"),
		    std::make_pair("foo","bar"),
		    true,
		   },
		   {
		    std::make_pair("foo","bar"),
		    std::make_pair("foo","bar////"),
		    true,
		   },
		   {
		    std::make_pair("foo","bar"),
		    std::make_pair("foo","baz"),
		    false,
		   },
		   {
		    std::make_pair("foo/baz","bar"),
		    std::make_pair("baz","bar/foo"),
		    true,
		   },
		   {
		    std::make_pair("../foo","bar"),
		    std::make_pair("../foo","baz"),
		    true,
		   },
	};

	for(const auto & d : data ) {
		auto aUID = fmp::TrackingDataDirectory::GetUID(d.A.first,d.A.second);
		auto bUID = fmp::TrackingDataDirectory::GetUID(d.B.first,d.B.second);
		EXPECT_EQ(aUID == bUID,d.Expected);
	}
}
