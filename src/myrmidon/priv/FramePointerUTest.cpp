#include "FramePointerUTest.hpp"

#include "RawFrame.hpp"

#include <google/protobuf/util/time_util.h>

#include "../utils/NotYetImplemented.hpp"

namespace fm = fort::myrmidon;
using namespace fort::myrmidon::priv;

google::protobuf::Timestamp fromEpoch(uint64_t sec) {
	time_t v = sec;
	return google::protobuf::util::TimeUtil::TimeTToTimestamp(v);
}





TEST_F(FramePointerUTest,CanBeFormatted) {

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
		auto a = RawFrame::Create(d.Path,pb,0);

		std::ostringstream os;
		os << a;
		EXPECT_EQ(os.str(),d.Expected);
	}

}
