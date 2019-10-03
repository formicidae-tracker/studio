#include "ProtobufExperimentReadWriterUTest.hpp"

#include "ProtobufExperimentReadWriter.hpp"

#include "TrackingDataDirectory.hpp"

#include "Experiment.pb.h"

#include <google/protobuf/util/message_differencer.h>

using namespace fort::myrmidon::priv;

TEST_F(ProtobufExperimentReadWriterUTest,IO) {
	fort::myrmidon::pb::TrackingDataDirectory pbTdd,encoded;
	pbTdd.set_path("foo.0001");
	pbTdd.set_startframe(5);
	pbTdd.set_endframe(8);
	ASSERT_TRUE(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:20.021-05:00",pbTdd.mutable_startdate()));
	ASSERT_TRUE(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:21.271-05:00",pbTdd.mutable_enddate()));

	auto tdd = ProtobufReadWriter::LoadTrackingDataDirectory(pbTdd);

	ASSERT_EQ(tdd.Path,"foo.0001");
	ASSERT_EQ(tdd.StartFrame,5);
	ASSERT_EQ(tdd.EndFrame,8);
	ASSERT_EQ(tdd.StartDate,pbTdd.startdate());
	ASSERT_EQ(tdd.EndDate,pbTdd.enddate());

	EXPECT_NO_THROW({ProtobufReadWriter::SaveTrackingDataDirectory(encoded,tdd);});
	ASSERT_EQ(google::protobuf::util::MessageDifferencer::Equals(encoded,pbTdd),true);

}
