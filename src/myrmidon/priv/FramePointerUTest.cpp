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

	std::vector<TestData> data;
	TestData a;a.A.Frame=0;a.B.Frame=0;a.Expected=false;
	data.push_back(a);

	a.B.Frame = 1;a.Expected=true;
	data.push_back(a);

	a.A.Frame=1;a.B.Frame=0;a.Expected=false;
	data.push_back(a);

	a.A.Path="a";a.A.PathStartDate=fromEpoch(1);
	a.B.Path="b";a.B.PathStartDate=fromEpoch(2);
	a.Expected=true;
	data.push_back(a);

	a.A.PathStartDate=fromEpoch(2);
	a.B.PathStartDate=fromEpoch(1);
	a.Expected=false;
	data.push_back(a);

	for(const auto & d : data ) {
		EXPECT_EQ(d.A<d.B,d.Expected);
	}
}




TEST_F(FramePointerUTest,CanBeFormatted) {
	struct TestData {
		FramePointer A;
		std::string Expected;
	};

	std::vector<TestData> data;
	TestData a;
	a.A.Frame = 0; a.Expected="/0";
	data.push_back(a);

	a.A.Frame = 2134; a.Expected="/2134";
	data.push_back(a);

	a.A.Path="foo";a.A.Frame = 42; a.Expected="foo/42";
	data.push_back(a);

	a.A.Path="foo/bar/baz";a.A.Frame = 42; a.Expected="foo/bar/baz/42";
	data.push_back(a);

	for(const auto & d : data ) {
		std::ostringstream os;
		os << d.A;
		EXPECT_EQ(os.str(),d.Expected);
	}


}
