#include "MovieSegmentUTest.hpp"

#include "MovieSegment.hpp"

#include "../TestSetup.hpp"

#include <fstream>

using namespace fort::myrmidon::priv;

TEST_F(MovieSegmentUTest,CanResolveFrameMatching) {

	MovieSegment::ListOfOffset offsets;
	offsets.push_back(std::make_pair(0,1234));
	offsets.push_back(std::make_pair(42,1234 + 2 ));
	offsets.push_back(std::make_pair(10,1234 + 1));

	MovieSegment::Ptr ms;
	EXPECT_THROW({
			ms = std::make_shared<MovieSegment>("foo",1234,1234+100+2,1,101,offsets);
		},std::invalid_argument);
	EXPECT_NO_THROW({
			ms = std::make_shared<MovieSegment>("foo",1234,1234+100+2,0,100,offsets);
		});

	EXPECT_EQ(ms->MovieFilepath(),"foo");

	EXPECT_EQ(ms->StartFrame(),1234);
	EXPECT_EQ(ms->EndFrame(),1234 + 100 + 2);

	EXPECT_EQ(ms->StartMovieFrame(),0);
	EXPECT_EQ(ms->EndMovieFrame(),100);

	std::sort(offsets.begin(),offsets.end());
	std::reverse(offsets.begin(),offsets.end());

	ASSERT_EQ(offsets.size(),ms->Offsets().size());
	for( size_t i = 0; i < offsets.size(); ++i) {
		EXPECT_EQ(offsets[i],ms->Offsets()[i]);
	}

	EXPECT_EQ(1234,ms->ToTrackingFrameID(0));
	EXPECT_EQ(1243,ms->ToTrackingFrameID(9));
	EXPECT_EQ(1245,ms->ToTrackingFrameID(10));
	EXPECT_EQ(1276,ms->ToTrackingFrameID(41));
	EXPECT_EQ(1278,ms->ToTrackingFrameID(42));
	EXPECT_EQ(1336,ms->ToTrackingFrameID(100));

	EXPECT_THROW({
			ms->ToTrackingFrameID(101);
		},std::out_of_range);


	EXPECT_EQ(0,ms->ToMovieFrameID(1234));
	EXPECT_EQ(9,ms->ToMovieFrameID(1243));
	EXPECT_EQ(10,ms->ToMovieFrameID(1244));
	EXPECT_EQ(10,ms->ToMovieFrameID(1245));
	EXPECT_EQ(41,ms->ToMovieFrameID(1276));
	EXPECT_EQ(42,ms->ToMovieFrameID(1277));
	EXPECT_EQ(42,ms->ToMovieFrameID(1278));
	EXPECT_EQ(100,ms->ToMovieFrameID(1336));

	EXPECT_THROW({
			ms->ToMovieFrameID(1233);
		},std::out_of_range);

	EXPECT_THROW({
			ms->ToMovieFrameID(1337);
		},std::out_of_range);
}



TEST_F(MovieSegmentUTest,CanBeParsed) {
	MovieSegment::ListOfOffset offsets;
	offsets.push_back(std::make_pair(0,1234));
	offsets.push_back(std::make_pair(42,1234 + 2 ));
	offsets.push_back(std::make_pair(10,1234 + 1));


	std::sort(offsets.begin(),offsets.end());

	fs::path matchFile = TestSetup::Basedir() / "frame-matching.txt";
	std::ofstream f(matchFile.c_str());
	MovieSegment::ListOfOffset::const_iterator it = offsets.begin();
	MovieSegment::ListOfOffset::const_iterator next = offsets.begin() + 1;

	for (size_t i = 0; i <= 100 ; ++i) {
		if (next != offsets.end() && next->first <= i ) {
			++it;
			next = it + 1;
		}
		f << i << " " << (i + it->second) << std::endl;
	}

	MovieSegment::Ptr ms;
	EXPECT_NO_THROW({
			ms = MovieSegment::Open("foo",matchFile);
		});

	std::reverse(offsets.begin(),offsets.end());
	ASSERT_EQ(offsets.size(),ms->Offsets().size());
	for(size_t i = 0; i < offsets.size(); ++i ) {
		EXPECT_EQ(offsets[i],ms->Offsets()[i]);
	}

	EXPECT_THROW({
			fs::path badMatchFile = TestSetup::Basedir() / "frame-matching-bad.txt";
			std::ofstream f(badMatchFile.c_str());
			f << "a 1234" << std::endl;
			MovieSegment::Open("foo",badMatchFile);
		},std::runtime_error);


	EXPECT_THROW({
			fs::path badMatchFile = TestSetup::Basedir() / "frame-matching-bad.txt";
			std::ofstream f(badMatchFile.c_str());
			f << "23 b" << std::endl;
			MovieSegment::Open("foo",badMatchFile);
		},std::runtime_error);

	EXPECT_THROW({
			MovieSegment::Open("foo",TestSetup::Basedir() / "does-not-exist.txt");
		},std::invalid_argument);

}
