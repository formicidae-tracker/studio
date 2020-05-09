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

// A geometric Shape
class Shape {
public:
	typedef std::shared_ptr<const priv::Shape> ConstPPtr;
	typedef std::shared_ptr<Shape>             Ptr;
	typedef std::shared_ptr<const Shape>       ConstPtr;
	typedef std::vector<ConstPtr>              ConstList;
	typedef std::vector<ConstPPtr>             ConstPList;
	virtual ~Shape();


	static ConstPtr Cast(const ConstPPtr & pShape);
	static ConstPPtr Cast(const ConstPtr & shape);
	static ConstPList Cast( const ConstList & shapes);
	static ConstList Cast( const ConstPList & pShapes);
};

// A circle
class Circle : public Shape {
public:
	Circle(const Eigen::Vector2d & center, double radius);
	virtual ~Circle();

	void SetCenter( const Eigen::Vector2d & center);

	const Eigen::Vector2d & Center() const;

	void SetRadius(double radius);

	double Radius() const;

	typedef std::shared_ptr<priv::Circle> PPtr;

	Circle(const PPtr & pCircle);

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
	Capsule(const Eigen::Vector2d & c1,
	        const Eigen::Vector2d & c2,
	        double r1,
	        double r2);
	virtual ~Capsule();

	void SetC1(const Eigen::Vector2d & c1);
	void SetC2(const Eigen::Vector2d & c2);
	const Eigen::Vector2d & C1() const;
	const Eigen::Vector2d & C2() const;

	void SetR1(double r1);
	void SetR2(double r1);
	double R1() const;
	double R2() const;

	typedef std::shared_ptr<priv::Capsule> PPtr;

	Capsule(const PPtr & pCapsule);

private:
	friend class Shape;

	PPtr d_p;
};

// A closed polygon
//
// A polygon is defined by a collection of <Vertex>. Polygon in
// myrmidon are always closed, meaning that there is no need to
// manually close it by setting <Vetex>(<Size>-1) == <Vertex>(0).
//
// Note that order matters as {(-1,-1),(1,-1),(1,1),(-1,1)} is a
// square, and {(-1,-1),(1,-1),(-1,1),(1,1)} is an hourglass.
class Polygon  : public Shape {
public:
	Polygon(const Vector2dList & vertices);
	virtual ~Polygon();

	size_t Size() const;

	const Eigen::Vector2d & Vertex(size_t i) const;

	void SetVertex(size_t i, const Eigen::Vector2d & v);

	typedef std::shared_ptr<priv::Polygon> PPtr;

	Polygon(const PPtr & pPolygon);
private:
	friend class Shape;

	PPtr d_p;
};


} // namespace myrmidon
} // namespace fort
