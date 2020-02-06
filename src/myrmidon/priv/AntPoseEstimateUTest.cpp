#include "AntPoseEstimateUTest.hpp"

#include "AntPoseEstimate.hpp"

#include <myrmidon/UtilsUTest.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(AntPoseEstimateUTest,HaveAnURI) {
	struct TestData {
		FrameReference Reference;
		TagID          TID;
		fs::path       Expected;
	};

	std::vector<TestData> testdata =
		{
		 {
		  FrameReference("foo",2,Time()),
		  123,
		  "foo/frames/2/estimates/123",
		 },
		};

	for (const auto & d: testdata ) {
		auto URI = AntPoseEstimate::BuildURI(d.Reference,d.TID);
		auto fromValues = AntPoseEstimate(d.Reference,
		                                  d.TID,
		                                  Eigen::Vector2d(),
		                                  0.0);
		auto fromVectors = AntPoseEstimate(d.Reference,
		                                   d.TID,
		                                   Eigen::Vector2d(1.0,0.0),
		                                   Eigen::Vector2d(2.0,0.0));


		EXPECT_EQ(fromValues.Reference().URI().generic_string(),
		          d.Reference.URI().generic_string());

		EXPECT_EQ(URI.generic_string(),
		          d.Expected.generic_string());
		EXPECT_EQ(fromValues.URI().generic_string(),
		          d.Expected.generic_string());

		EXPECT_EQ(fromVectors.URI().generic_string(),
		          d.Expected.generic_string());

		EXPECT_EQ(fromValues.TargetTagID(),
		          d.TID);

		EXPECT_EQ(fromVectors.TargetTagID(),
		          d.TID);

	}
}

TEST_F(AntPoseEstimateUTest,CanBeComputedFromPoints) {
	struct TestData {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		Eigen::Vector2d Head,Tail,Result;
		double Angle;
	};

	std::vector<TestData> testdata =
		{
		 {
		  Eigen::Vector2d(1,1),Eigen::Vector2d(-1,-1),Eigen::Vector2d(0,0),
		  M_PI / 4.0,
		 },
		 {
		  Eigen::Vector2d(1,1),Eigen::Vector2d(11,1),Eigen::Vector2d(6,1),
		  M_PI,
		 }

		};

	for (const auto & d : testdata) {
		AntPoseEstimate res(FrameReference("foo",0,Time()),
		                    123,
		                    d.Head,
		                    d.Tail);
		EXPECT_TRUE(VectorAlmostEqual(res.PositionFromTag(),
		                              d.Result));

		EXPECT_DOUBLE_EQ(res.AngleFromTag(),d.Angle);

	}

	//sub pixel distances between point should throw exception
	EXPECT_THROW({
			AntPoseEstimate(FrameReference(),
			                123,
			                Eigen::Vector2d(0,0.5),
			                Eigen::Vector2d(0,0));
		},std::invalid_argument);

}

TEST_F(AntPoseEstimateUTest,CanComputeMeanPose) {
	struct TestData {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		Eigen::Vector2d              ExpectedPosition;
		double                       ExpectedAngle;
		std::vector<AntPoseEstimate::ConstPtr> Poses;
	};
	FrameReference ref;
	std::vector<TestData> testdata =
		{
		 {
		  Eigen::Vector2d(0,0),
		  0.0,
		  {
		   std::make_shared<AntPoseEstimate>(ref,0,Eigen::Vector2d(1,0),  0.1 * M_PI),
		   std::make_shared<AntPoseEstimate>(ref,0,Eigen::Vector2d(-1,0),-0.1 * M_PI),
		  },
		 },
		 {
		  Eigen::Vector2d(0,0),
		  0.0,
		  {
		   std::make_shared<AntPoseEstimate>(ref,0,Eigen::Vector2d(1,0),  0.1 * M_PI),
		   std::make_shared<AntPoseEstimate>(ref,0,Eigen::Vector2d(-1,0), 2.1 * M_PI),
		  },
		 },

		 {
		  Eigen::Vector2d(0,0),
		  0.0,
		  {},
		 },

		};

	for (const auto & d : testdata) {
		Eigen::Vector2d res;
		double angle;
		EXPECT_NO_THROW(AntPoseEstimate::ComputeMeanPose(res,angle,
		                                                 d.Poses.begin(),
		                                                 d.Poses.end()));

		EXPECT_TRUE(VectorAlmostEqual(res,d.ExpectedPosition));
		EXPECT_NEAR(angle,d.ExpectedAngle,1.0e-8);
	}
}



} // namespace priv
} // namespace myrmidon
} // namespace fort
