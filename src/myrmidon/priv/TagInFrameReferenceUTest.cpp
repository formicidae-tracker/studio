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
		   {"",0,0,"/frames/0/tags/0"},
		   {"",2134,34,"/frames/2134/tags/34"},
		   {"foo",42,43,"foo/frames/42/tags/43"},
		   {"foo/bar/baz",42,56,"foo/bar/baz/frames/42/tags/56"},
	};

	if (fs::path::preferred_separator == '\\') {
		data.push_back({"foo\bar\baz",42,103,"foo/bar/baz/frames/42/tags/103"});
	}

	for(const auto & d : data ) {
		FrameReference a(d.Path.generic_string(),
		                 d.FID,
		                 fort::myrmidon::Time::FromTimeT(0));
		TagInFrameReference t(a,d.TID);
		fs::path expectedParentPath(d.Path.generic_string().empty() ? "/" : d.Path);
		std::ostringstream os;
		os << t;
		EXPECT_EQ(os.str(),d.Expected);
		auto expectedURI = expectedParentPath / "frames" /std::to_string(d.FID) / "tags" / std::to_string(d.TID);
		EXPECT_EQ(t.URI().generic_string(), expectedURI.generic_string());
	}



}
