#pragma once

#include <Eigen/Core>

#include "ForwardDeclaration.hpp"
#include "Types.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class Shape {
public:
	typedef std::shared_ptr<Shape>       Ptr;
	typedef std::shared_ptr<const Shape> ConstPtr;

	enum class Type {
		Capsule = 0,
		Circle  = 1,
		Polygon = 2
	};

	Shape(Type type);
	virtual ~Shape();

	Type ShapeType() const;

	virtual bool Contains(const Eigen::Vector2d & point) const = 0;

	virtual AABB ComputeAABB() const = 0;

	static CapsuleConstPtr ToCapsule(const ConstPtr & s);
	static CircleConstPtr  ToCircle(const ConstPtr & s);
	static PolygonConstPtr ToPolygon(const ConstPtr & s);
private:
	Type d_type;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
