#include "TrackingDataDirectoryUTest.hpp"


#include "TrackingDataDirectory.hpp"
#include <fort/myrmidon/Experiment.pb.h>
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/util/message_differencer.h>

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/myrmidon/UtilsUTest.hpp>
#include <fort/myrmidon/utils/NotYetImplemented.hpp>

#include "RawFrame.hpp"
#include "TagStatisticsUTest.hpp"

#include <yaml-cpp/yaml.h>

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(TrackingDataDirectoryUTest,ExtractInfoFromTrackingDatadirectories) {

	try {
		auto tddPath = TestSetup::Basedir() / "foo.0001";
		auto startOpen = Time::Now();

		auto tdd = TrackingDataDirectory::Open(tddPath,TestSetup::Basedir());
		auto endOpen = Time::Now();
		std::cerr << "Opening " <<  tddPath << " took " << endOpen.Sub(startOpen) << std::endl;
		EXPECT_EQ(tdd->URI(),"foo.0001");
		EXPECT_EQ(tdd->StartFrame(),0);
		EXPECT_EQ(tdd->EndFrame(),999);
		EXPECT_TRUE(TimeEqual(tdd->StartDate(),TestSetup::StartTime("foo.0001/tracking.0000.hermes")));
		EXPECT_TRUE(TimeEqual(tdd->EndDate(),TestSetup::EndTime("foo.0001/tracking.0009.hermes")));

		std::vector<SegmentIndexer<std::string>::Segment> segments;

		for(size_t i = 0; i < 10; ++i ) {
			std::ostringstream os;
			os << "tracking." << std::setw(4) << std::setfill('0') << i << ".hermes";
			segments.push_back(std::make_pair(FrameReference("foo.0001",
			                                                 100*i,
			                                                 TestSetup::StartTime("foo.0001/" + os.str())),
			                                  os.str()));
		}
		ASSERT_EQ(segments.size(),tdd->TrackingSegments().Segments().size());
		for(size_t i = 0;  i < segments.size(); ++i) {
			// Can make mistakes about path extraction quite easily
			EXPECT_EQ(segments[i].first.URI(),
			          tdd->TrackingSegments().Segments()[i].first.URI());
			EXPECT_EQ(segments[i].first.FrameID(),tdd->TrackingSegments().Segments()[i].first.FrameID());
			EXPECT_TRUE(TimeEqual(segments[i].first.Time(),tdd->TrackingSegments().Segments()[i].first.Time()));
			EXPECT_EQ(segments[i].second,tdd->TrackingSegments().Segments()[i].second);
		}



		uint64_t i  = tdd->StartFrame();
		auto iterStart = Time::Now();

		for ( auto it = tdd->begin(); it != tdd->end() ; ++it) {
			auto f = *it;
			EXPECT_EQ(f->Frame().FrameID(),i);
			ASSERT_EQ(f->Tags().size(),2);
			EXPECT_EQ(f->Tags().Get(0).id(),123);
			EXPECT_EQ(f->Tags().Get(1).id(),124);
			++i;
		}
		auto iterEnd = Time::Now();
		std::cerr << "Iterating over all frames from " <<  tddPath << " took " << iterEnd.Sub(iterStart) << std::endl;
		i = tdd->EndFrame()-3;
		for( auto it = tdd->FrameAt(tdd->EndFrame()-3); it != tdd->end(); ++it ) {
			EXPECT_EQ((*it)->Frame().FrameID(),i);
			ASSERT_EQ((*it)->Tags().size(),2);
			EXPECT_EQ((*it)->Tags().Get(0).id(),123);
			EXPECT_EQ((*it)->Tags().Get(1).id(),124);
			++i;
		}



	} catch( const std::exception & e) {
		ADD_FAILURE() << "Got unexpected exception: " << e.what();
	}

	EXPECT_THROW({
			//no tracking data
			auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "bar.0000",TestSetup::Basedir());
		}, std::invalid_argument);

	EXPECT_THROW({
			//directory does not exists
			auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.1234",TestSetup::Basedir());
		}, std::invalid_argument);


	EXPECT_THROW({
			// is not a directory
			auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "test.myrmidon",TestSetup::Basedir());
		}, std::invalid_argument);

	EXPECT_THROW({
			//root does not exist
			auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0001", TestSetup::Basedir() /  "does-not-exists");
		}, std::invalid_argument);

	EXPECT_THROW({
			//no configuration
			auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir()/ "no-config.0000",TestSetup::Basedir() );
		},YAML::BadFile);


}


TEST_F(TrackingDataDirectoryUTest,HasUIDBasedOnPath) {
	struct TestData {
		fs::path A,B;
		bool Expected;
	};

	std::vector<TestData> data
		= {
		   {
		    "bar/foo",
		    "bar/foo",
		    true,
		   },
		   {
		    "bar/foo",
		    "bar////foo",
		    true,
		   },
		   {
		    "bar/foo",
		    "baz/foo",
		    false,
		   },
		   {
		    "bar/../foo",
		    "baz/../foo",
		    true,
		   },
	};

	for(const auto & d : data ) {
		auto aUID = TrackingDataDirectory::GetUID(d.A);
		auto bUID = TrackingDataDirectory::GetUID(d.B);
		EXPECT_EQ(aUID == bUID,d.Expected);
	}
}


TEST_F(TrackingDataDirectoryUTest,HaveConstructorChecks) {
	uint64_t startFrame = 10;
	uint64_t endFrame = 20;
	auto startTime = Time::Parse("2019-11-02T22:02:24.674+01:00");
	auto endTime = Time::Parse("2019-11-02T22:02:25.783+01:00");
	auto segments = std::make_shared<TrackingDataDirectory::TrackingIndex>();
	auto movies = std::make_shared<TrackingDataDirectory::MovieIndex>();
	auto cache = std::make_shared<TrackingDataDirectory::FrameReferenceCache>();
	auto absolutePath = TestSetup::Basedir() / "bar";
	EXPECT_NO_THROW({
			TrackingDataDirectory::Create("foo",absolutePath,startFrame,endFrame,startTime,endTime,segments,movies,cache);
		});

	EXPECT_THROW({
			TrackingDataDirectory::Create("foo","bar",startFrame,endFrame,startTime,endTime,segments,movies,cache);
		},std::invalid_argument);

	EXPECT_THROW({
			TrackingDataDirectory::Create("foo",absolutePath,endFrame,startFrame,startTime,endTime,segments,movies,cache);
		},std::invalid_argument);

	EXPECT_THROW({
			TrackingDataDirectory::Create("foo",absolutePath,startFrame,endFrame,endTime,startTime,segments,movies,cache);
		},std::invalid_argument);

}

TEST_F(TrackingDataDirectoryUTest,AlmostRandomAccess) {
	auto tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir());
	EXPECT_NO_THROW({
			FrameID middle = (tdd->StartFrame() + tdd->EndFrame()) / 2;

			tdd->FrameReferenceAt(middle);
	});

	EXPECT_THROW({
			tdd->FrameReferenceAt(tdd->EndFrame()+1);
		},std::out_of_range);

	EXPECT_NO_THROW({
			auto iter = tdd->FrameAfter(tdd->StartDate());
			EXPECT_EQ(iter,tdd->begin());
			auto next = tdd->FrameAfter(tdd->StartDate().Add(1));
			EXPECT_EQ(++iter,next);
			auto iterLast = tdd->FrameAfter(tdd->EndDate().Add(-1));
			EXPECT_EQ((*iterLast)->Frame().FrameID(),tdd->EndFrame());
			auto iterEnd = tdd->FrameAfter(tdd->EndDate());
			EXPECT_EQ(iterEnd,tdd->end());
		});

	EXPECT_THROW({
			auto iterEnd = tdd->FrameAfter(tdd->StartDate().Add(-1));
		}, std::out_of_range);

	EXPECT_NO_THROW({
			auto ref = tdd->FrameReferenceAfter(tdd->StartDate());
			EXPECT_EQ(ref.FrameID(),tdd->StartFrame());
			ref = tdd->FrameReferenceAfter(tdd->StartDate().Add(1));
			EXPECT_EQ(ref.FrameID(),tdd->StartFrame()+1);
		});

}

TEST_F(TrackingDataDirectoryUTest,CanBeFormatted) {
	TrackingDataDirectory::Ptr foo;
	EXPECT_NO_THROW({
			foo = TrackingDataDirectory::Open(TestSetup::Basedir()/"foo.0000",TestSetup::Basedir()/"bar.0000");
		});
	std::ostringstream oss;
	oss << *foo;
	EXPECT_EQ(oss.str(),
	          "TDD{URI:'../foo.0000', start:2019-11-02T09:00:20.021Z, end:2019-11-02T09:02:00.848126001Z}");

}


::testing::AssertionResult ApriltagOptionsEqual(const tags::ApriltagOptions & a,
                                                const tags::ApriltagOptions & b) {
	if ( a.Family != b.Family ) {
		return ::testing::AssertionFailure() << "a.Family=" << tags::GetFamilyName(a.Family)
		                                     << " and b.Family=" << tags::GetFamilyName(b.Family)
		                                     << " differs";
	}
#define MY_ASSERT_FLOAT(fieldName) do {	  \
		auto fieldName ## Assertion = ::testing::internal::CmpHelperFloatingPointEQ<float>("a." #fieldName, \
			             "b." #fieldName, \
			             a.fieldName, \
			             b.fieldName); \
		if ( fieldName ## Assertion == false ) { \
			return fieldName ## Assertion; \
		} \
	}while(0)
#define MY_ASSERT_OTHER(fieldName) do {	  \
		if ( a.fieldName != b.fieldName ) { \
			return ::testing::AssertionFailure() << "a." << #fieldName << "= " << std::boolalpha << a.fieldName \
			                                     << " and b." << #fieldName << "= " << std::boolalpha << b.fieldName \
			                                     << " differs"; \
		} \
	}while(0)


	MY_ASSERT_FLOAT(QuadDecimate);
	MY_ASSERT_FLOAT(QuadSigma);
	MY_ASSERT_OTHER(RefineEdges);
	MY_ASSERT_OTHER(QuadMinClusterPixel);
	MY_ASSERT_OTHER(QuadMaxNMaxima);
	MY_ASSERT_FLOAT(QuadCriticalRadian);
	MY_ASSERT_FLOAT(QuadMaxLineMSE);
	MY_ASSERT_OTHER(QuadMinBWDiff);
	MY_ASSERT_OTHER(QuadDeglitch);

#undef MY_ASSERT_FLOAT
#undef MY_ASSERT_OTHER
	return ::testing::AssertionSuccess();
}

TEST_F(TrackingDataDirectoryUTest,ParsesDetectionSettings) {
	TrackingDataDirectory::Ptr foo[2];
	EXPECT_NO_THROW({
			foo[0] = TrackingDataDirectory::Open(TestSetup::Basedir()/"foo.0000",TestSetup::Basedir());
			foo[1] = TrackingDataDirectory::Open(TestSetup::Basedir()/"foo.0001",TestSetup::Basedir());
		});
	tags::ApriltagOptions expected;
	EXPECT_TRUE(ApriltagOptionsEqual(foo[1]->DetectionSettings(),expected));

	expected.Family = tags::Family::Tag36h11;
	expected.QuadMinClusterPixel = 25;
	expected.QuadMinBWDiff = 75;
	EXPECT_TRUE(ApriltagOptionsEqual(foo[0]->DetectionSettings(),expected));
}


TEST_F(TrackingDataDirectoryUTest,ComputesAndCacheTagStatistics) {
	TrackingDataDirectory::Ptr tdd;
	ASSERT_NO_THROW({
			tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "computed-cache-test.0000",TestSetup::Basedir());
		});

	EXPECT_FALSE(tdd->TagStatisticsComputed());
	EXPECT_THROW({
			tdd->TagStatistics();
		},TrackingDataDirectory::ComputedRessourceUnavailable);

	TagStatisticsHelper::Timed computedStats,cachedStats;
	try {
		auto loaders = tdd->PrepareTagStatisticsLoaders();
		EXPECT_EQ(loaders.size(),10);
		for ( const auto & l : loaders ) {
			l();
		}

		EXPECT_TRUE(tdd->TagStatisticsComputed());
		computedStats = tdd->TagStatistics();
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Computation should not throw this excption: " << e.what();
	}

	tdd.reset();
	ASSERT_NO_THROW({
			tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "computed-cache-test.0000",TestSetup::Basedir());
			cachedStats = tdd->TagStatistics();
		});
	EXPECT_TRUE(tdd->TagStatisticsComputed());
	EXPECT_TRUE(TimedEqual(cachedStats,computedStats));

}

TEST_F(TrackingDataDirectoryUTest,ComputesAndCacheFullFrames) {
	TrackingDataDirectory::Ptr tdd;
	ASSERT_NO_THROW({
			tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "computed-cache-test.0000",TestSetup::Basedir());
		});

	EXPECT_FALSE(tdd->FullFramesComputed());
	EXPECT_THROW({
			tdd->FullFrames();
		},TrackingDataDirectory::ComputedRessourceUnavailable);

	try {
		auto loaders = tdd->PrepareFullFramesLoaders();
		EXPECT_EQ(loaders.size(),1);
		for ( const auto & l : loaders ) {
			l();
		}
		EXPECT_EQ(tdd->FullFrames().size(),1);
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Computation should not throw this exception: " << e.what();
	}

	EXPECT_TRUE(tdd->FullFramesComputed());
	ASSERT_NO_THROW({
			tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "computed-cache-test.0000",TestSetup::Basedir());
			EXPECT_EQ(tdd->FullFrames().size(),1);
		});
	EXPECT_TRUE(tdd->FullFramesComputed());

}

TEST_F(TrackingDataDirectoryUTest,CanListTagCloseUpFiles) {
	auto files = TrackingDataDirectory::ListTagCloseUpFiles(TestSetup::Basedir() / "foo.0000/ants");
	auto expectedFiles = TestSetup::CloseUpFilesForPath(TestSetup::Basedir() / "foo.0000");
	ASSERT_EQ(files.size(),expectedFiles.size());
	for (const auto & [frameID,ff] : files ) {
		auto fi = expectedFiles.find(ff.first);
		if ( fi == expectedFiles.end()) {
			ADD_FAILURE() << "Returned unexpected file " << ff.first.generic_string();
		} else {
			if ( !ff.second != !fi->second ) {
				ADD_FAILURE() << "Filtering mismatch for file " << ff.first.generic_string();
			} else if (ff.second) {
				EXPECT_EQ(*ff.second,*fi->second);
			}
		}
	}
	files = TrackingDataDirectory::ListTagCloseUpFiles(TestSetup::Basedir() / "computed-cache-test.0000/ants");
	ASSERT_EQ(files.count(0),1);
	auto f = files.find(0)->second;
	EXPECT_EQ(f.first,TestSetup::Basedir() / "computed-cache-test.0000/ants/ant_0_frame_0.png");
	ASSERT_FALSE(!f.second);
	EXPECT_EQ(*f.second,0);
}

TEST_F(TrackingDataDirectoryUTest,ComputesAndCacheTagCloseUps) {
	TrackingDataDirectory::Ptr tdd;
	ASSERT_NO_THROW({
			tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "computed-cache-test.0000",TestSetup::Basedir());
		});

	EXPECT_FALSE(tdd->TagCloseUpsComputed());
	EXPECT_THROW({
			tdd->TagCloseUps();
		},TrackingDataDirectory::ComputedRessourceUnavailable);

	TagCloseUp::ConstPtr computed,cached;

	try {
		auto loaders = tdd->PrepareTagCloseUpsLoaders();
		EXPECT_EQ(loaders.size(),1);
		for ( const auto & l : loaders ) {
			l();
		}
		ASSERT_EQ(tdd->TagCloseUps().size(),1);
		computed = tdd->TagCloseUps()[0];
	} catch ( const std::exception & e) {
		ADD_FAILURE() << "Computation should not throw this exception: " << e.what();
	}

	EXPECT_TRUE(tdd->TagCloseUpsComputed());
	ASSERT_NO_THROW({
			tdd = TrackingDataDirectory::Open(TestSetup::Basedir() / "computed-cache-test.0000",TestSetup::Basedir());
			ASSERT_EQ(tdd->TagCloseUps().size(),1);
			cached = tdd->TagCloseUps()[0];
		});
	EXPECT_TRUE(tdd->TagCloseUpsComputed());
	EXPECT_EQ(computed->URI(), "computed-cache-test.0000/frames/0/closeups/0x000");
	EXPECT_EQ(computed->AbsoluteFilePath(), TestSetup::Basedir() / "computed-cache-test.0000/ants/ant_0_frame_0.png");
	EXPECT_EQ(computed->TagValue(), 0);

	EXPECT_EQ(cached->URI(), "computed-cache-test.0000/frames/0/closeups/0x000");
	EXPECT_EQ(cached->AbsoluteFilePath(), TestSetup::Basedir() / "computed-cache-test.0000/ants/ant_0_frame_0.png");
	EXPECT_EQ(cached->TagValue(), 0);

	EXPECT_TRUE(VectorAlmostEqual(computed->TagPosition(),cached->TagPosition()));
	for (int i = 0; i < 4; ++i ) {
		EXPECT_TRUE(VectorAlmostEqual(computed->Corners()[i],cached->Corners()[i]));
	}

	EXPECT_DOUBLE_EQ(computed->TagAngle(),cached->TagAngle());

}

} // namespace fort
} // namespace myrmidon
} // namespace priv
