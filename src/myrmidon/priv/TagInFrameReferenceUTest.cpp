#include "TagInFrameReferenceUTest.hpp"

#include "TagInFrameReference.hpp"

TEST_F(TagInFrameReferenceUTest,CanBeFormatted) {
	using namespace fort::myrmidon::priv;
	struct TestData {
		fs::path Path;
		FrameID FID;
		TagID   TID;
		std::string Expected;
	};

	std::vector<TestData> data
		= {
		   {"",0,0,"/0/0"},
		   {"",2134,34,"/2134/34"},
		   {"foo",42,43,"foo/42/43"},
		   {"foo/bar/baz",42,56,"foo/bar/baz/42/56"},
	};

	if (fs::path::preferred_separator == '\\') {
		data.push_back({"foo\bar\baz",42,103,"foo/bar/baz/42/103"});
	}

	for(const auto & d : data ) {
		FrameReference a(d.Path,
		                 d.FID,
		                 fort::myrmidon::Time::FromTimeT(0));
		TagInFrameReference t(a,d.TID);
		std::ostringstream os;
		os << t;
		EXPECT_EQ(os.str(),d.Expected);
		EXPECT_EQ(t.Path(),d.Path / std::to_string(d.FID) / std::to_string(d.TID) );
	}



}
