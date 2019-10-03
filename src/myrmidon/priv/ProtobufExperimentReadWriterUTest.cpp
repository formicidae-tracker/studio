#include "ProtobufExperimentReadWriterUTest.hpp"

#include <google/protobuf/util/message_differencer.h>

#include "Experiment.pb.h"

#include "ProtobufExperimentReadWriter.hpp"

#include "TrackingDataDirectory.hpp"
#include "FramePointer.hpp"



using namespace fort::myrmidon::priv;


TEST_F(ProtobufExperimentReadWriterUTest,TrackingDataDirectoryIO) {
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


TEST_F(ProtobufExperimentReadWriterUTest,FramePointerIO) {
	struct TestData {
		FramePointer A;
	};

	std::vector<TestData> data
		= {
		   TestData{.A={.Frame=0}},
		   TestData{.A={.Frame=1}},
		   TestData{.A={.Path="a",
		                .PathStartDate=google::protobuf::util::TimeUtil::TimeTToTimestamp(1),
		                .Frame=1}},
		   TestData{.A={.Path="b",
		                .PathStartDate=google::protobuf::util::TimeUtil::TimeTToTimestamp(2),
		                .Frame=0}},
	};


	for(const auto & d : data ) {
		fort::myrmidon::pb::FramePointer pb;
		ASSERT_NO_THROW(ProtobufReadWriter::SaveFramePointer(pb,d.A));
		EXPECT_EQ(d.A.Frame,pb.frame());
		EXPECT_EQ(d.A.Path,pb.path());
		EXPECT_EQ(d.A.PathStartDate,pb.pathstartdate());

		auto decoded = ProtobufReadWriter::LoadFramePointer(pb);

		if ( d.A.Path.empty() ) {
			EXPECT_EQ(decoded.get(),(FramePointer*)NULL);
			continue;
		}

		EXPECT_EQ(decoded->Path,d.A.Path);
		EXPECT_EQ(decoded->Frame,d.A.Frame);
		EXPECT_EQ(decoded->PathStartDate,d.A.PathStartDate);
	}

}
