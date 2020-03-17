#include "CapsuleUTest.hpp"

#include <random>
#include <fstream>


#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


#include "Capsule.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(CapsuleUTest,Type) {
	Capsule capsule(Eigen::Vector2d(0,0),Eigen::Vector2d(0,0),0,0);
	EXPECT_EQ(capsule.ShapeType(),Shape::Type::Capsule);
}

TEST_F(CapsuleUTest,PointCollision) {
	Capsule capsule(Eigen::Vector2d(0,0),Eigen::Vector2d(0,1),1.0,0.01);

	struct TestData {
		double X,Y;
		bool   Expected;
	};

	std::vector<TestData> testdata =
		{
		 {0,0,true},
		 {0,1,true},
		 {1,0,true},
		 {0.5-1.0e-6,0.5-1.0e-6,true},
		 {0.1,1,false},
		};
	for ( const auto & d : testdata ) {
		EXPECT_EQ(capsule.Contains(Eigen::Vector2d(d.X,d.Y)),d.Expected)
			<< "Testing (" << d.X << "," << d.Y << ")";
	}

}

TEST_F(CapsuleUTest,TestCaspuleCollision) {
	struct TestData {
		double aC1X,aC1Y,aC2X,aC2Y,aR1,aR2;
		double bC1X,bC1Y,bC2X,bC2Y,bR1,bR2;
		bool Expected;
	};


	std::vector<TestData> testdata =
		{
		 //Toy example with square positions
		 {
		  0,0,0,1,0.25,0.25,
		  1,0,1,1,0.25,0.25,
		  false,
		 },
		 {
		  0,0,0,1,0.6,0.6,
		  1,0,1,1,0.6,0.6,
		  true,
		 },
		 {
		  0,0,0,1,0.55,0.35,
		  1,0,1,1,0.35,0.55,
		  false,
		 },
		 {
		  0,0,0,1,0.35,0.55,
		  1,0,1,1,0.55,0.35,
		  false,
		 },
		 {
		  0,0,0,1,0.35,0.55,
		  1,0,1,1,0.35,0.55,
		  true,
		 },
		 {
		  0,0,0,1,0.55,0.35,
		  1,0,1,1,0.55,0.35,
		  true,
		 },
		 // More complicated example, where intersection is not on the
		 // minimal distance between segments
		 {
		  0,0,0,1,0.3,0.7,
		  1,0.1,1.2,1.2,0.3,0.7,
		  true,
		 },
		 //Another edge case found by playing with real shapes
		 {
		  0.00,0.00,0.00,1.00,0.02,0.30,
		  0.30,0.00,0.60,0.90,0.02,0.33,
		  true
		 }
		};

	for ( const auto & d : testdata ) {
		Capsule a(Eigen::Vector2d(d.aC1X,d.aC1Y),
		          Eigen::Vector2d(d.aC2X,d.aC2Y),
		          d.aR1,
		          d.aR2);

		Capsule b(Eigen::Vector2d(d.bC1X,d.bC1Y),
		          Eigen::Vector2d(d.bC2X,d.bC2Y),
		          d.bR1,
		          d.bR2);

		bool res = Capsule::Intersect(a.C1(),a.C2(),a.R1(),a.R2(),
		                              b.C1(),b.C2(),b.R1(),b.R2());
		EXPECT_EQ(res,d.Expected) << " Intersecting " << a << " and " << b;
	}

}






} // namespace priv
} // namespace myrmidon
} // namespace fort
