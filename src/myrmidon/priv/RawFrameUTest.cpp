#include "RawFrameUTest.hpp"

#include "RawFrame.hpp"

#include <google/protobuf/util/time_util.h>

#include "../UtilsUTest.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

google::protobuf::Timestamp fromEpoch(uint64_t sec) {
	time_t v = sec;
	return google::protobuf::util::TimeUtil::TimeTToTimestamp(v);
}





TEST_F(RawFrameUTest,CanBeFormatted) {

	struct TestData {
		fs::path Path;
		uint64_t FID;
		std::string Expected;
	};

	std::vector<TestData> data
		= {
		   {"",0,"/0"},
		   {"",2134,"/2134"},
		   {"foo",42,"foo/42"},
		   {"foo/bar/baz",42,"foo/bar/baz/42"},
	};




	if (fs::path::preferred_separator == '\\') {
		data.push_back({"foo\bar\baz",42,"foo/bar/baz/42"});
	}


	for(const auto & d : data ) {
		fort::hermes::FrameReadout pb;
		pb.set_frameid(d.FID);
		auto a = RawFrame::Create(d.Path.generic_string(),pb,0);

		std::ostringstream os;
		os << *a;
		EXPECT_EQ(os.str(),d.Expected);
		EXPECT_EQ(a->Basepath(),d.Path);
		EXPECT_EQ(a->Path(),d.Path / std::to_string(d.FID));
	}

}


TEST_F(RawFrameUTest,ExtractsDataFromHermes) {
	uint64_t frameID = 1234;
	google::protobuf::Timestamp frameTime;
	google::protobuf::util::TimeUtil::FromString("2019-11-02T10:00:20.021+01:00", &frameTime);
	Time::MonoclockID mID = 123;
	uint64_t timestampUS = 123456;
	auto time = Time::FromTimestampAndMonotonic(frameTime, timestampUS * 1000, mID);
	int width = 640;
	int height = 480;

	fort::hermes::FrameReadout withData,withError;
	withData.set_frameid(1234);
	withData.mutable_time()->CheckTypeAndMergeFrom(frameTime);
	withData.set_timestamp(timestampUS);
	withData.set_width(width);
	withData.set_height(height);
	for (int i = 0; i < 10; ++i) {
		auto a  = withData.add_tags();
		a->set_id(i);
		a->set_x(10*i);
		a->set_y(15*i);
	}
	withError.set_frameid(1234);
	withError.mutable_time()->CheckTypeAndMergeFrom(frameTime);
	withError.set_timestamp(timestampUS);
	withError.set_width(width);
	withError.set_height(height);
	withError.set_error(fort::hermes::FrameReadout_Error_PROCESS_OVERFLOW);

	std::vector<RawFrame::ConstPtr> results;

	for (auto  m : {&withData,&withError}) {
		auto res = RawFrame::Create("foo",*m,mID);
		EXPECT_EQ(res->FrameID(),frameID);
		EXPECT_TRUE(TimeEqual(res->Time(),time));
		EXPECT_EQ(res->Width(),width);
		EXPECT_EQ(res->Height(),height);
		results.push_back(res);
	}
	auto withDataRes = results[0];
	auto withErrorRes = results[1];

	EXPECT_EQ(withErrorRes->Tags().size(),0);
	EXPECT_EQ(withErrorRes->Error(),fort::hermes::FrameReadout_Error_PROCESS_OVERFLOW);

	EXPECT_EQ(withDataRes->Error(),fort::hermes::FrameReadout_Error_NO_ERROR);
	ASSERT_EQ(withDataRes->Tags().size(),10);
	for (int i = 0; i < 10; ++i) {
		auto t = withDataRes->Tags().Get(i);
		EXPECT_EQ(t.id(),i);
		EXPECT_EQ(t.x(),10*i);
		EXPECT_EQ(t.y(),15*i);
	}

	// Creating a FrameReadout "consume" the tag data in the upstream message
	EXPECT_EQ(withData.tags_size(),0);

}

} // namespace fort
} // namespace myrmidon
} // namespace priv
