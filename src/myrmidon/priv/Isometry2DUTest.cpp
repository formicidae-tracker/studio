#include "Isometry2DUTest.hpp"

#include "Isometry2D.hpp"

using namespace fort::myrmidon::priv;


TEST_F(Isometry2DUTest,TestInverse) {
	struct TestData {
		Eigen::Vector3d TranslationRotation;
	};

	std::vector<TestData> data
		= {
		   TestData{.TranslationRotation=Eigen::Vector3d(0,0,0)},
		   TestData{.TranslationRotation=Eigen::Vector3d(1,-2,M_PI/23.0)},
		   TestData{.TranslationRotation=Eigen::Vector3d(42.561,-5987.0,69.96*M_PI)},
	};

	for(const auto & d : data ) {
		auto isometry = Isometry2Dd(d.TranslationRotation.z(),d.TranslationRotation.block<2,1>(0,0));
		auto res = isometry.inverse() * isometry;
		EXPECT_DOUBLE_EQ(res.angle(),0.0);
		EXPECT_DOUBLE_EQ(res.translation().x(),0.0);
		EXPECT_DOUBLE_EQ(res.translation().y(),0.0);
		auto rot = res.rotation().matrix();
		EXPECT_DOUBLE_EQ(rot(0,0),1.0);
		EXPECT_DOUBLE_EQ(rot(1,1),1.0);
		EXPECT_DOUBLE_EQ(rot(0,1),0.0);
		EXPECT_DOUBLE_EQ(rot(1,0),0.0);

	}
}


::testing::AssertionResult IsModulo2Pi(double v) {
	if ( v < -M_PI ) {
		return ::testing::AssertionFailure() << v << " is smaller than " << -M_PI;
	}
	if ( v >= M_PI ) {
		return ::testing::AssertionFailure() << v << " is  greater or equal to " << M_PI;
	}
	return ::testing::AssertionSuccess();
}


TEST_F(Isometry2DUTest,AngleIsModulo2Pi) {
	struct TestData {
		double Angle;
	};

	std::vector<TestData> data
		= {
		   TestData{.Angle=0.0},
		   TestData{.Angle=3.0 * M_PI},
		   TestData{.Angle=2.0 * M_PI},
		   TestData{.Angle=-1.0 * M_PI},
		   TestData{.Angle=-2.0 * M_PI},
		   TestData{.Angle=-3.0 * M_PI},
		   TestData{.Angle=std::nan("")},
	};

	for(const auto & d : data ) {
		auto isometry = Isometry2Dd(d.Angle,Eigen::Vector2d::Zero());
		EXPECT_TRUE(IsModulo2Pi(isometry.angle()));
	}
}


TEST_F(Isometry2DUTest,CanBeUsedToTransformPoint) {
	struct TestData {
		double Angle,Tx,Ty,Sx,Sy,Ex,Ey;
	};

	std::vector<TestData> data
		= {
		   TestData{0.0,0.0,0.0,2.0,3.0,2.0,3.0},
		   TestData{M_PI/2.0,1.0,-1.0,2.0,3.0,-2.0,1.0},
		   TestData{-M_PI/2.0,1.0,-1.0,2.0,3.0,4.0,-3.0},
	};

	for (const auto & d : data ) {
		Isometry2Dd isometry(d.Angle,Eigen::Vector2d(d.Tx,d.Ty));
		auto res = isometry * Eigen::Vector2d(d.Sx,d.Sy);
		EXPECT_DOUBLE_EQ(res.x(),d.Ex);
		EXPECT_DOUBLE_EQ(res.y(),d.Ey);
	}


}
