#include "TagCloseUpUTest.hpp"

#include "TagCloseUp.hpp"

#include <myrmidon/TestSetup.hpp>
#include <myrmidon/UtilsUTest.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


TEST_F(TagCloseUpUTest,CanBeFormatted) {
	struct TestData {
		fs::path Path;
		FrameID FID;
		TagID   TID;
		std::string Expected;
	};

	std::vector<TestData> data
		= {
		   {"",0,0,"/frames/0/closeups/0"},
		   {"",2134,34,"/frames/2134/closeups/34"},
		   {"foo",42,43,"foo/frames/42/closeups/43"},
		   {"foo/bar/baz",42,56,"foo/bar/baz/frames/42/closeups/56"},
	};

	if (fs::path::preferred_separator == '\\') {
		data.push_back({"foo\bar\baz",42,103,"foo/bar/baz/frames/42/closeups/103"});
	}

	TagCloseUp::Vector2dList corners =
		{
		 Eigen::Vector2d(0,0),
		 Eigen::Vector2d(0,0),
		 Eigen::Vector2d(0,0),
		 Eigen::Vector2d(0,0),
		};

	for(const auto & d : data ) {
		FrameReference a(d.Path.generic_string(),
		                 d.FID,
		                 fort::myrmidon::Time::FromTimeT(0));
		TagCloseUp t(TestSetup::Basedir() / "foo", a,d.TID,Eigen::Vector2d::Zero(),0.0,corners);
		fs::path expectedParentPath(d.Path.generic_string().empty() ? "/" : d.Path);
		std::ostringstream os;
		os << t;
		EXPECT_EQ(os.str(),d.Expected);
		auto expectedURI = expectedParentPath / "frames" /std::to_string(d.FID) / "closeups" / std::to_string(d.TID);
		EXPECT_EQ(t.URI().generic_string(), expectedURI.generic_string());
	}



}


TEST_F(TagCloseUpUTest,CanComputeGeometricValues) {
	TagCloseUp::Vector2dList corners =
		{
		 Eigen::Vector2d(1.0,1.0),
		 Eigen::Vector2d(2.0,1.0),
		 Eigen::Vector2d(2.0,2.0),
		 Eigen::Vector2d(1.0,2.0)
		};

	Eigen::Vector2d center = (corners[0] + corners[1] + corners[2] + corners[3] ) / 4.0;
	std::vector<double> angles =
		{
		 0.0,
		 M_PI / 3.0,
		 M_PI / 5.0,
		 -3.0 * M_PI / 4,
		};

	for ( const auto &  a : angles ) {
		Isometry2Dd trans(a,Eigen::Vector2d(2.0,1.0));
		TagCloseUp::Vector2dList transCorners =
			{
			 trans * corners[0],
			 trans * corners[1],
			 trans * corners[2],
			 trans * corners[3]
		};
		double res = TagCloseUp::ComputeAngleFromCorners(transCorners[0],
		                                                 transCorners[1],
		                                                 transCorners[2],
		                                                 transCorners[3]);
		EXPECT_DOUBLE_EQ(res,a);

		auto tcu = TagCloseUp(TestSetup::Basedir() / "foo.png",
		                      FrameReference("",0,Time()),
		                      0,
		                      trans * center,
		                      res,
		                      transCorners);

		EXPECT_DOUBLE_EQ(tcu.TagSizePx(),1.0);
		EXPECT_DOUBLE_EQ(tcu.Squareness(),1.0);

		auto expectedImageToTag = Isometry2Dd(a,trans * center).inverse();
		EXPECT_DOUBLE_EQ(expectedImageToTag.angle(),tcu.ImageToTag().angle());
		EXPECT_TRUE(VectorAlmostEqual(expectedImageToTag.translation(),
		                              tcu.ImageToTag().translation()));

	}

}

TEST_F(TagCloseUpUTest,CanBeLoadedFromFiles) {
	auto files = TagCloseUp::Lister::ListFiles(TestSetup::Basedir() / "foo.0000/ants");
	auto expectedFiles = TestSetup::CloseUpFilesForPath(TestSetup::Basedir() / "foo.0000");
	ASSERT_EQ(files.size(),expectedFiles.size());
	for (const auto & [FID,ff] : files ) {
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
	auto resolver = [](FrameID) -> FrameReference {
		                return FrameReference("",0,Time());
	                };

	auto barAntDir = TestSetup::Basedir() / "bar.0000/ants";
	auto lister = TagCloseUp::Lister::Create(barAntDir,
	                                         tags::Family::Tag36h11,
	                                         80,
	                                         resolver);
	auto loaders = lister->PrepareLoaders();
	ASSERT_EQ(loaders.size(),1);
	TagCloseUp::ConstPtr computed;
	auto res = loaders[0]();
	ASSERT_EQ(res.size(),1);
	EXPECT_EQ(res[0]->TagValue(),0);
	computed = res[0];

	auto cachePath = TagCloseUp::Lister::CacheFilePath(barAntDir);
	ASSERT_TRUE(fs::is_regular_file(cachePath)) <<
		cachePath << " does not exist";


	EXPECT_THROW({
			// wrong family, won't load from cache
			TagCloseUp::Lister::Create(barAntDir,
			                           tags::Family::Tag36ARTag,
			                           80,
			                           resolver,
			                           true);
		},std::runtime_error);

	EXPECT_THROW({
			// wrong threshold, won't load from cache
			TagCloseUp::Lister::Create(barAntDir,
			                           tags::Family::Tag36h11,
			                           90,
			                           resolver,
			                           true);
		},std::runtime_error);

	ASSERT_NO_THROW({
			auto fromCache = TagCloseUp::Lister::Create(barAntDir,
			                                            tags::Family::Tag36h11,
			                                            80,
			                                            resolver,
			                                            true);
			loaders = fromCache->PrepareLoaders();

		});
	ASSERT_EQ(loaders.size(),1);
	auto cachedList = loaders[0]();
	ASSERT_EQ(cachedList.size(),1);
	auto cached = cachedList[0];

	EXPECT_EQ(computed->Frame().URI().generic_string(),
	          cached->Frame().URI().generic_string());

	EXPECT_EQ(computed->AbsoluteFilePath().generic_string(),
	          cached->AbsoluteFilePath().generic_string());

	EXPECT_EQ(computed->TagValue(),
	          cached->TagValue());

	EXPECT_TRUE(VectorAlmostEqual(computed->TagPosition(),
	                              cached->TagPosition()));

	EXPECT_DOUBLE_EQ(computed->TagAngle(),
	                 cached->TagAngle());

	for (size_t i = 0; i < 4; ++i) {
		EXPECT_TRUE(VectorAlmostEqual(computed->Corners()[i],
		                              cached->Corners()[i]));
	}

}


TEST_F(TagCloseUpUTest,ClassInvariants) {

	EXPECT_THROW({
			// not an absolute path
	              TagCloseUp("foo",
	                         FrameReference(),
	                         0,
	                         Eigen::Vector2d(),
	                         0.0,
	                         {
	                          Eigen::Vector2d(),
	                          Eigen::Vector2d(),
	                          Eigen::Vector2d(),
	                          Eigen::Vector2d(),
	                         });

		}, std::invalid_argument);

		EXPECT_THROW({
				// Not having 4 corners
				TagCloseUp(TestSetup::Basedir() / "foo",
	                         FrameReference(),
	                         0,
	                         Eigen::Vector2d(),
	                         0.0,
	                         {
	                          Eigen::Vector2d(),
	                          Eigen::Vector2d(),
	                          Eigen::Vector2d(),
	                         });

		}, std::invalid_argument);

}


} // namespace priv
} // namespace myrmidon
} // namespace fort