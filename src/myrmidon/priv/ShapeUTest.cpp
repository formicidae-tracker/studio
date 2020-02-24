#include "ShapeUTest.hpp"

#include "Circle.hpp"
#include "Capsule.hpp"
#include "Polygon.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(ShapeUTest,Casting) {
	auto circle = std::make_shared<Circle>(Eigen::Vector2d(0,0),0);
	auto capsule = std::make_shared<Capsule>(Eigen::Vector2d(0,0),Eigen::Vector2d(0,0),0,0);
	auto polygon = std::make_shared<Polygon>(Polygon({{0,0},{0,0},{0,0}}));

	EXPECT_FALSE(!Shape::ToCircle(circle));
	EXPECT_TRUE(!Shape::ToCapsule(circle));
	EXPECT_TRUE(!Shape::ToPolygon(circle));

	EXPECT_TRUE(!Shape::ToCircle(capsule));
	EXPECT_FALSE(!Shape::ToCapsule(capsule));
	EXPECT_TRUE(!Shape::ToPolygon(capsule));

	EXPECT_TRUE(!Shape::ToCircle(polygon));
	EXPECT_TRUE(!Shape::ToCapsule(polygon));
	EXPECT_FALSE(!Shape::ToPolygon(polygon));

}



} // namespace priv
} // namespace myrmidon
} // namespace fort
