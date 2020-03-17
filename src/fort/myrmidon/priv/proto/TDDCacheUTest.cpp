#include "TDDCacheUTest.hpp"

#include "TDDCache.hpp"

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

#include "IOUtils.hpp"

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {


TEST_F(TDDCacheUTest,CahceIO) {

	fs::path cacheURI = "cache-test.0000";

	EXPECT_THROW({
			// Should be an absolute path as first argument
			TDDCache::Load("foo.0000","foo.0000");
		},std::invalid_argument);

	EXPECT_THROW({
			//Was never opened, so there is no cache
			TDDCache::Load(TestSetup::Basedir() / cacheURI ,cacheURI.generic_string());
		},std::runtime_error);

	TrackingDataDirectory::ConstPtr opened,cached;
	ASSERT_NO_THROW({
			//will open it one first, and saving the cache
			opened = TrackingDataDirectory::Open(TestSetup::Basedir() / cacheURI,
			                            TestSetup::Basedir());
			cached = TDDCache::Load(TestSetup::Basedir() / cacheURI, cacheURI.generic_string());
		});


	//Craft a special cache to throw an exception
	pb::TrackingDataDirectory h;
	IOUtils::SaveFrameReference(h.mutable_start(),
	                            FrameReference(cached->URI(),
	                                           cached->StartFrame(),
	                                           cached->StartDate()));
	IOUtils::SaveFrameReference(h.mutable_end(),
	                            FrameReference(cached->URI(),
	                                           cached->EndFrame(),
	                                           cached->EndDate()));
	std::vector<TDDCache::ReadWriter::LineWriter> lines=
		{
		 [&cached](pb::TrackingDataDirectoryFileLine & line) {

			 IOUtils::SaveMovieSegment(line.mutable_movie(),
			                           cached->MovieSegments().Segments().begin()->second,
			                           cached->AbsoluteFilePath());
		 },
		};
	auto cacheFilepath = TestSetup::Basedir() / cacheURI / TDDCache::CACHE_FILENAME;
	ASSERT_NO_THROW({
			TDDCache::ReadWriter::Write(cacheFilepath,
			                            h,
			                            lines);
		});

	EXPECT_THROW({
			TDDCache::Load(TestSetup::Basedir() / cacheURI, cacheURI.generic_string());
		},std::runtime_error);


	ASSERT_NO_THROW({
			fs::remove_all(cacheFilepath);
		});



}


} // namespace proto
} // namespace priv
} // namespace myrmidon
} // namespace fort
