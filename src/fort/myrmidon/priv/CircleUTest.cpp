#include "CircleUTest.hpp"


#include <fort/myrmidon/Shapes.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


TEST_F(CircleUTest,Type) {
	Circle circle(Eigen::Vector2d(0,0),0);
	EXPECT_EQ(circle.ShapeType(),myrmidon::Shape::Type::Circle);
}


TEST_F(CircleUTest,PointCollision) {
	struct TestData {
		double X,Y;
		Circle C;
		bool Expected;
	};

	std::vector<TestData> testdata =
		{
		 {
		  0,0,
		  Circle({0,0},1e-6),
		  true,
		 },
		 {
		  0,1-1e-6,
		  Circle({0,0},1),
		  true,
		 },
		 {
		  std::sqrt(2)/2,std::sqrt(2)/2,
		  Circle({0,0},1),
		  false,
		 },
		 {
		  std::sqrt(2)/2-1.0e-6,std::sqrt(2)/2-1.0e-6,
		  Circle({0,0},1),
		  true,
		 },
		};

	for ( const auto & d : testdata ) {
		EXPECT_EQ(d.C.Contains(Eigen::Vector2d(d.X,d.Y)),d.Expected);
	}

}




} // namespace priv
} // namespace myrmidon
} // namespace fort
