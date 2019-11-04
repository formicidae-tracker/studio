#include "ProtobufExperimentReadWriterUTest.hpp"

#include <google/protobuf/util/message_differencer.h>

#include <myrmidon/Experiment.pb.h>

#include "ProtobufExperimentReadWriter.hpp"

#include "TrackingDataDirectory.hpp"



using namespace fort::myrmidon::priv;


TEST_F(ProtobufExperimentReadWriterUTest,TrackingDataDirectoryIO) {
	fort::myrmidon::pb::TrackingDataDirectory pbTdd,encoded;
	pbTdd.set_path("foo.0001");
	pbTdd.set_startframe(5);
	pbTdd.set_endframe(8);
	ASSERT_TRUE(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:20.021-05:00",pbTdd.mutable_startdate()->mutable_timestamp()));
	ASSERT_TRUE(google::protobuf::util::TimeUtil::FromString("1972-01-01T10:00:21.271-05:00",pbTdd.mutable_enddate()->mutable_timestamp()));

	auto tdd = ProtobufReadWriter::LoadTrackingDataDirectory(pbTdd,"foo");

	ASSERT_EQ(tdd.Path(),"foo.0001");
	ASSERT_EQ(tdd.StartFrame(),5);
	ASSERT_EQ(tdd.EndFrame(),8);
	ASSERT_EQ(tdd.StartDate().ToTimestamp(),pbTdd.startdate().timestamp());
	ASSERT_EQ(tdd.EndDate().ToTimestamp(),pbTdd.enddate().timestamp());

	EXPECT_NO_THROW({ProtobufReadWriter::SaveTrackingDataDirectory(encoded,tdd);});
	ASSERT_EQ(google::protobuf::util::MessageDifferencer::Equals(encoded,pbTdd),true);

}
