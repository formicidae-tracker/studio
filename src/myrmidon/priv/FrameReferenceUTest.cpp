#include "FrameReferenceUTest.hpp"

#include "FrameReference.hpp"

TEST_F(FrameReferenceUTest,CanBeFormatted) {

	struct TestData {
		fs::path Path;
		fs::path ExpectedParentPath;
		uint64_t FID;
		std::string Expected;
	};

	std::vector<TestData> data
		= {
		   {"","/",0,"/frames/0"},
		   {"","/",2134,"/frames/2134"},
		   {"foo","foo",42,"foo/frames/42"},
		   {"foo/bar/baz","foo/bar/baz",42,"foo/bar/baz/frames/42"},
	};


	if (fs::path::preferred_separator == '\\') {
		data.push_back({"foo\bar\baz","foo/bar/baz",42,"foo/bar/baz/frames/42"});
	}


	for(const auto & d : data ) {
		fort::myrmidon::priv::FrameReference a(d.Path.generic_string(),
		                                       d.FID,
		                                       fort::myrmidon::Time::FromTimeT(0));
		std::ostringstream os;
		os << a;

		EXPECT_EQ(os.str(),d.Expected);

		EXPECT_EQ(a.ParentURI(),d.ExpectedParentPath);
		auto expectedURI = d.ExpectedParentPath / "frames" / std::to_string(d.FID);
		EXPECT_EQ(a.URI(), expectedURI);
	}

}
