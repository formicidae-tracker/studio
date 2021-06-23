#include "ShapeUTest.hpp"

#include <fort/myrmidon/Shapes.hpp>

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(ShapeUTest,Type) {
	auto circle = std::make_unique<Circle>(Eigen::Vector2d(0,0),0);
	auto capsule = std::make_unique<Capsule>(Eigen::Vector2d(0,0),Eigen::Vector2d(0,0),0,0);
	auto polygon = std::make_unique<Polygon>(Polygon({{0,0},{0,0},{0,0}}));

	EXPECT_EQ(circle->ShapeType(),fort::myrmidon::Shape::Type::CIRCLE);
	EXPECT_EQ(capsule->ShapeType(),fort::myrmidon::Shape::Type::CAPSULE);
	EXPECT_EQ(polygon->ShapeType(),fort::myrmidon::Shape::Type::POLYGON);



}



} // namespace priv
} // namespace myrmidon
} // namespace fort
