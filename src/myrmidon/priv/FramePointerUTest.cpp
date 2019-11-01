#include "FramePointerUTest.hpp"

#include "FramePointer.hpp"
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
		FramePointer::ConstPtr A;
		std::string Expected;
	};

	std::vector<TestData> data
		= {
		   {FramePointer::Create("",0,fm::Time()),"/0"},
		   {FramePointer::Create("",2134,fm::Time()),"/2134"},
		   {FramePointer::Create("foo",42,fm::Time()),"foo/42"},
		   {FramePointer::Create("foo/bar/baz",42,fm::Time()),"foo/bar/baz/42"},
	};




	if (fs::path::preferred_separator == '\\') {
		data.push_back({FramePointer::Create("foo\bar\baz",42,fm::Time()),"foo/bar/baz/42"});
	}


	for(const auto & d : data ) {
		std::ostringstream os;
		os << *d.A;
		EXPECT_EQ(os.str(),d.Expected);
	}

}
