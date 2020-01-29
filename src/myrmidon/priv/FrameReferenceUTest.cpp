#include "FrameReferenceUTest.hpp"

#include "FrameReference.hpp"

TEST_F(FrameReferenceUTest,CanBeFormatted) {

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
		fort::myrmidon::priv::FrameReference a(d.Path.generic_string(),
		                                       d.FID,
		                                       fort::myrmidon::Time::FromTimeT(0));

		std::ostringstream os;
		os << a;
		EXPECT_EQ(os.str(),d.Expected);
		EXPECT_EQ(a.ParentPath(),d.Path);
		EXPECT_EQ(a.Path(),d.Path / std::to_string(d.FID));
	}

}
