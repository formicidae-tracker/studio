#pragma once

#include <Eigen/Core>

#include <fort/myrmidon/Shapes.hpp>

#include "ForwardDeclaration.hpp"
#include "Types.hpp"




namespace fort {
namespace myrmidon {
namespace priv {

class Shape {
public:
	typedef std::shared_ptr<Shape>       Ptr;
	typedef std::shared_ptr<const Shape> ConstPtr;

	Shape(myrmidon::Shape::Type type);
	virtual ~Shape();

	myrmidon::Shape::Type ShapeType() const;

	virtual bool Contains(const Eigen::Vector2d & point) const = 0;

	virtual AABB ComputeAABB() const = 0;

	static priv::CapsuleConstPtr ToCapsule(const priv::Shape::ConstPtr & s);
	static priv::CircleConstPtr  ToCircle(const priv::Shape::ConstPtr & s);
	static priv::PolygonConstPtr ToPolygon(const priv::Shape::ConstPtr & s);


private:
	myrmidon::Shape::Type d_type;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
