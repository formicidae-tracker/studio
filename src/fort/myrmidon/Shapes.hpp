#pragma once

#include <memory>

#include "Types.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Shape;
// private <fort::myrmidon::priv> implementation
class Circle;
// private <fort::myrmidon::priv> implementation
class Capsule;
// private <fort::myrmidon::priv> implementation
class Polygon;
}

// Base class for  geometric Shape
//
// Base class for geometruc Shape such as <Circle>, <Capsule> and
// <Polygon>. This class is only here to be able to construct
// hereterogenous <ConstList> of shape, and contains only static
// methods to convert from and to opaque Shape.
class Shape {
public:
	// An opaque private pointer
	typedef std::shared_ptr<const priv::Shape> ConstPPtr;
	// A pointer to a Shape
	typedef std::shared_ptr<Shape>             Ptr;
	// A const pointer to a Shape
	typedef std::shared_ptr<const Shape>       ConstPtr;
	// A const list of Shape
	typedef std::vector<ConstPtr>              ConstList;
	// An opaque const list Shape
	typedef std::vector<ConstPPtr>             ConstPList;
	// Default destructor
	virtual ~Shape();

	// Casts from opaque pointer.
	// @pShape an opaque Shape
	//
	// @return a <ConstPtr> of the Shape
	static ConstPtr Cast(const ConstPPtr & pShape);

	// Casts to opaque pointer.
	// @shape a Shape
	//
	// @return a <ConstPPtr> to the opaque implementation
	static ConstPPtr Cast(const ConstPtr & shape);
	// Cast from opaque list
	// @pShapes an opaque list of shape
	//
	// @return a <ConstList> of Shape
	static ConstList Cast( const ConstPList & pShapes);

	// Cast to opaque list
	// @shapes a list of shape
	//
	// @return an opaque list of shapes
	static ConstPList Cast( const ConstList & shapes);

};

// A circle
class Circle : public Shape {
public:
	// public constructor
	// @center the center of the circle
	// @radius the radius of the circle
	Circle(const Eigen::Vector2d & center, double radius);
	// virtual destructor
	virtual ~Circle();

	// Sets the center of the circle
	// @center the center of the circle
	void SetCenter( const Eigen::Vector2d & center);

	// Gets the center of the circle
	//
	// @return a reference to the center of the circle
	const Eigen::Vector2d & Center() const;

	// Sets the radius of the circle
	// @radius the radius of the circle
	void SetRadius(double radius);

	// Gets the radius of the circle
	//
	// @return the circle's radius
	double Radius() const;

	// Opaque pointer to implementation
	typedef std::shared_ptr<priv::Circle> PPtr;

	// Private implementation constructor.
	// @pCircle opaque pointer to implementation
	Circle(const PPtr & pCircle);

	// Downcast to private implementation.
	//
	// @return the private implementation
	const PPtr & ToPrivate() const;

private:
	friend class Shape;
	PPtr d_p;
};

// A capsule
//
// A Capsule is two circle and the region in between thos two circles.
//
// Their main purpose is to define <Ant> body parts.
class Capsule  : public Shape {
public:
	// A const pointer to a Capsule
	typedef std::shared_ptr<const Capsule> ConstPtr;
	// public constructor
	// @c1 the first center
	// @c2 the second center
	// @r1 the radius at <c1>
	// @r2 the radius at <c2>
	Capsule(const Eigen::Vector2d & c1,
	        const Eigen::Vector2d & c2,
	        double r1,
	        double r2);
	// virtual destructor
	virtual ~Capsule();
	// Sets the first center
	// @c1 the first center
	void SetC1(const Eigen::Vector2d & c1);
	// Sets the second center
	// @c2 the second center
	void SetC2(const Eigen::Vector2d & c2);
	// Gets the first center
	//
	// @return the first center
	const Eigen::Vector2d & C1() const;
	// Gets the second center
	//
	// @return the second center
	const Eigen::Vector2d & C2() const;
	// Sets the radius at C1
	// @r1 the radius at <C1>
	void SetR1(double r1);
	// Sets the radius at C2
	// @r2 the radius at <C2>
	void SetR2(double r2);
	// Gets the radius at C1
	//
	// @return the radius at <C1>
	double R1() const;
	// Gets the radius at C2
	//
	// @return the radius at <C2>
	double R2() const;

	// Opaque pointer to implementation
	typedef std::shared_ptr<priv::Capsule> PPtr;

	// Private implementation constructor.
	// @pCapsule opaque pointer to implementation
	Capsule(const PPtr & pCapsule);

	// Downcast to private implementation.
	//
	// @return the private implementation
	const PPtr & ToPrivate() const;

private:
	friend class Shape;

	PPtr d_p;
};

// A closed polygon
//
// A polygon is defined by a collection of <Vertex>. Polygon in
// myrmidon are always closed, meaning that there is no need to
// manually close it by setting <Vertex>(<Size>-1) == <Vertex>(0).
//
// Note that order matters as {(-1,-1),(1,-1),(1,1),(-1,1)} is a
// square, and {(-1,-1),(1,-1),(-1,1),(1,1)} is an hourglass.
class Polygon  : public Shape {
public:
	// Public constructor
	// @vertices the vertices of the polygon
	Polygon(const Vector2dList & vertices);
	// virtual destructor
	virtual ~Polygon();

	// Gets the size of the Polygon
	//
	// @return the number of vertices in the Polygon
	size_t Size() const;

	// Gets a polygon vertex
	// @i the index of the wanted vertex in [0;<Size>-1]
	//
	// @return a const reference to the wanted vertex
	const Eigen::Vector2d & Vertex(size_t i) const;

	// Sets a polygon vertex
	// @i the index of the vertex to modify in [0,<Size>-1]
	// @v the wanted value for the vertex
	void SetVertex(size_t i, const Eigen::Vector2d & v);

	// Opaque pointer to implementation
	typedef std::shared_ptr<priv::Polygon> PPtr;

	// Private implementation constructor
	// @pPolygon opaque pointer to implementation
	Polygon(const PPtr & pPolygon);

	// Downcast to private implementation.
	//
	// @return the private implementation
	const PPtr & ToPrivate() const;

private:
	friend class Shape;

	PPtr d_p;
};


} // namespace myrmidon
} // namespace fort
