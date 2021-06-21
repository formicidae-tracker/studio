#include "PolygonUTest.hpp"

#include <fort/myrmidon/Shapes.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(PolygonUTest,Type) {
	Polygon p({{0,0},{0,0},{0,0}});
	EXPECT_EQ(p.ShapeType(),myrmidon::Shape::Type::Polygon);
};

TEST_F(PolygonUTest,PointCollision) {
	struct TestData {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		Eigen::Vector2d Point;
		Polygon         Poly;
		bool            Expected;
	};

	std::vector<TestData> testdata =
		{
		 // with a square
		 {
		  Eigen::Vector2d({0,0}),
		  Polygon({{-1,-1},{1,-1},{1,1},{-1,1}}),
		  true,
		 },
		 {
		  Eigen::Vector2d({1,2}),
		  Polygon({{-1,-1},{1,-1},{1,1},{-1,1}}),
		  false,
		 },
		 {
		  Eigen::Vector2d({1,-2}),
		  Polygon({{-1,-1},{1,-1},{1,1},{-1,1}}),
		  false,
		 },
		 {
		  Eigen::Vector2d({0.5,1-1.0e-6}),
		  Polygon({{-1,-1},{1,-1},{1,1},{-1,1}}),
		  true,
		 },
		 // with a extinction rebellion shape
		 {
		  // the center of the shape is now outside
		  Eigen::Vector2d({0.0,0.0}),
		  Polygon({{-1,-1},{1,-1},{-1,1},{1,1}}),
		  false,
		 },
		 {
		  // just above the center is inside
		  Eigen::Vector2d({0.0,1.0e-6}),
		  Polygon({{-1,-1},{1,-1},{-1,1},{1,1}}),
		  true,
		 },
		 {
		  // just right the center is outside
		  Eigen::Vector2d({1.0e-6,0.0}),
		  Polygon({{-1,-1},{1,-1},{-1,1},{1,1}}),
		  false,
		 },

		};

	for ( const auto & d : testdata ) {
		EXPECT_EQ(d.Poly.Contains(d.Point),d.Expected);
	}


}


} // namespace priv
} // namespace myrmidon
} // namespace fort
