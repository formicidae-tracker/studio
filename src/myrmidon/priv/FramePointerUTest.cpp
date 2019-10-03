#include "FramePointerUTest.hpp"

#include "FramePointer.hpp"
#include <google/protobuf/util/time_util.h>

using namespace fort::myrmidon::priv;

google::protobuf::Timestamp fromEpoch(uint64_t sec) {
	time_t v = sec;
	return google::protobuf::util::TimeUtil::TimeTToTimestamp(v);
}


TEST_F(FramePointerUTest,CanBeOrdered) {
	struct TestData {
		FramePointer A,B;
		bool Expected;
	};

	std::vector<TestData> data
		= {
		   //if smae path, compare frame number
		   TestData{.A={.Frame=0},.B={.Frame=0},.Expected=false},
		   TestData{.A={.Frame=0},.B={.Frame=1},.Expected=true},
		   TestData{.A={.Frame=1},.B={.Frame=0},.Expected=false},
		   // if different paths, compare path start date
		   TestData{.A={.Path="a",.PathStartDate=fromEpoch(1),.Frame=1},
		            .B={.Path="b",.PathStartDate=fromEpoch(2),.Frame=0},
		            .Expected=true},
		   TestData{.A={.Path="a",.PathStartDate=fromEpoch(2),.Frame=1},
		            .B={.Path="b",.PathStartDate=fromEpoch(1),.Frame=0},
		            .Expected=false},
	};

	for(const auto & d : data ) {
		EXPECT_EQ(d.A<d.B,d.Expected);
	}
}




TEST_F(FramePointerUTest,CanBeFormatted) {
	struct TestData {
		FramePointer A;
		std::string Expected;
	};

	std::vector<TestData> data
		= {
		   TestData{.A={.Frame=0},.Expected="/0"},
		   TestData{.A={.Frame=2134},.Expected="/2134"},
		   TestData{.A={.Path="foo",.Frame=42},.Expected="foo/42"},
		   TestData{.A={.Path="foo/bar/baz",.Frame=0},.Expected="foo/bar/baz/0"},
	};

	for(const auto & d : data ) {
		std::ostringstream os;
		os << d.A;
		EXPECT_EQ(os.str(),d.Expected);
	}


}
