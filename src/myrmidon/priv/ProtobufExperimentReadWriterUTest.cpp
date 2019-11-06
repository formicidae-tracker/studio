#include "ProtobufExperimentReadWriterUTest.hpp"

#include <google/protobuf/util/message_differencer.h>

#include <myrmidon/Experiment.pb.h>

#include "ProtobufExperimentReadWriter.hpp"

#include "TrackingDataDirectory.hpp"

#include "../UtilsUTest.hpp"


namespace fort {
namespace myrmidon {
namespace priv {




TEST_F(ProtobufExperimentReadWriterUTest,TimeIO) {
	google::protobuf::Timestamp pbt;
	google::protobuf::util::TimeUtil::FromString("2019-01-T10:12:34.567+01:00",&pbt);

	struct TestData {
		Time     T;
		int64_t  Seconds;
		int64_t  Nanos;
		uint64_t Mono;
	};

	std::vector<TestData> data
		= {
		   {Time(),0,0,0},
		   {Time::FromTimeT(1234),1234,0,0},
		   {Time::FromTimestamp(pbt),pbt.seconds(),pbt.nanos(),0},
		   {
		    Time::FromTimestampAndMonotonic(pbt, 123456789, 42),
		    pbt.seconds(),
		    pbt.nanos(),
		    123456789
		   },
	};

	for (const auto & d : data ) {
		pb::Time t;
		pb::Time expected;
		expected.mutable_timestamp()->set_seconds(d.Seconds);
		expected.mutable_timestamp()->set_nanos(d.Nanos);
		expected.set_monotonic(d.Mono);
		ProtobufReadWriter::SaveTime(t, d.T);
		EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(t,expected));
		auto res = ProtobufReadWriter::LoadTime(t, 42);
		EXPECT_TRUE(TimeEqual(res,d.T));
	}

}


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

}
}
}
