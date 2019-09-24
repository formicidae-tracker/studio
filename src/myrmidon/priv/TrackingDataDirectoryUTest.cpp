#include "TrackingDataDirectoryUTest.hpp"


#include "TrackingDataDirectory.hpp"
#include "Experiment.pb.h"
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/message_differencer.h>

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


TEST_F(TrackingDataDirectoryUTest,IO) {
	fort::myrmidon::pb::TrackingDataDirectory pbTdd,encoded;
	pbTdd.set_path("foo.0001");
	pbTdd.set_startframe(5);
	pbTdd.set_endframe(8);
	ASSERT_TRUE(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:20.021-05:00",pbTdd.mutable_startdate()));
	ASSERT_TRUE(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:21.271-05:00",pbTdd.mutable_enddate()));

	auto tdd = fmp::TrackingDataDirectory::FromSaved(pbTdd);

	ASSERT_EQ(tdd.Path,"foo.0001");
	ASSERT_EQ(tdd.StartFrame,5);
	ASSERT_EQ(tdd.EndFrame,8);
	ASSERT_EQ(tdd.StartDate,pbTdd.startdate());
	ASSERT_EQ(tdd.EndDate,pbTdd.enddate());


	tdd.Encode(encoded);
	ASSERT_EQ(google::protobuf::util::MessageDifferencer::Equals(encoded,pbTdd),true);

}
