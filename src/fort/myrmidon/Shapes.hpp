#pragma once

#include <memory>

#include "Types.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
template <typename T> class Isometry2D;


typedef Isometry2D<double> Isometry2Dd;
}

/**
 * Base class for  geometric Shape
 *
 * Base class for geometruc Shape such as Circle, Capsule and
 * Polygon. This class is only here to be able to construct
 * hereterogenous List of shape
 */
class Shape {
public:
	// A pointer to a Shape
	typedef std::shared_ptr<Shape> Ptr;
	// A list of Shape
	typedef std::vector<Ptr>       List;

	// The type of a Shape.
	enum class Type {
	    // A <Capsule>
		Capsule = 0,
		// A <Circle>
		Circle  = 1,
		// A <Polygon>
		Polygon = 2
	};

	// Default destructor
	virtual ~Shape();

	// Gets the Shape <Type>
	//
	// @return the <Type> of the Shape
	Type ShapeType() const;

	virtual bool Contains(const Eigen::Vector2d & point) const = 0;

	virtual AABB ComputeAABB() const = 0;

	virtual std::unique_ptr<Shape> Clone() const = 0;

protected:
	Shape(Type type);

	Type  d_type;
};

// A circle
class Circle : public Shape {
public:
	// A pointer to a Capsule
	typedef std::shared_ptr<Circle>       Ptr;

	// public constructor
	// @center the center of the circle
	// @radius the radius of the circle
	//
	// R version:
	// ```R
	// c <- fmCircleCreate(c(x,y),r)
	// ```
	Circle(const Eigen::Vector2d & center, double radius);
	// virtual destructor
	virtual ~Circle();

	// Sets the center of the circle
	// @center the center of the circle
	//
	// R version:
	// ```R
	// c$setCenter(c(x,y))
	// ```
	void SetCenter( const Eigen::Vector2d & center);

	// Gets the center of the circle
	//
	// R version:
	// ```R
	// c$center()
	// ```
	//
	// @return a reference to the center of the circle
	const Eigen::Vector2d & Center() const;

	// Sets the radius of the circle
	// @radius the radius of the circle
	//
	// R version:
	// ```R
	// c$setRadius(radius)
	// ```
	void SetRadius(double radius);

	// Gets the radius of the circle
	//
	// R version:
	// ```R
	// c$radius()
	// ```
	//
	// @return the circle's radius
	double Radius() const;


	bool Contains(const Eigen::Vector2d & point) const override;

	AABB ComputeAABB() const override;

	std::unique_ptr<Shape> Clone() const override;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
private:
	Eigen::Vector2d d_center;
	double d_radius;
};

// A capsule
//
// A Capsule is two circle and the region in between thos two circles.
//
// Their main purpose is to define <Ant> body parts.
class Capsule  : public Shape {
public:
	// A pointer to a Capsule
	typedef std::shared_ptr<Capsule>       Ptr;

	Capsule();

	// public constructor
	// @c1 the first center
	// @c2 the second center
	// @r1 the radius at <c1>
	// @r2 the radius at <c2>
	//
	//
	// R version:
	// ```R
	// c <- fmCapsuleCreate(c(x1,y1),c(x2,y2),r1,r2)
	// ```
	Capsule(const Eigen::Vector2d & c1,
	        const Eigen::Vector2d & c2,
	        double r1,
	        double r2);

	// virtual destructor
	virtual ~Capsule();
	// Sets the first center
	// @c1 the first center
	//
	// R version:
	// ```R
	// c$setC1(c(x1,y1))
	// ```
	void SetC1(const Eigen::Vector2d & c1);
	// Sets the second center
	// @c2 the second center
	//
	// R version:
	// ```R
	// c$setC2(c(x2,y2))
	// ```
	void SetC2(const Eigen::Vector2d & c2);
	// Gets the first center
	//
	// R version:
	// ```R
	// c$c1()
	// ```
	//
	// @return the first center
	inline const Eigen::Vector2d & C1() const {
		return d_c1;
	}
	// Gets the second center
	//
	// R version:
	// ```R
	// c$c2()
	// ```
	//
	// @return the second center
	inline const Eigen::Vector2d & C2() const {
		return d_c2;
	}
	// Sets the radius at C1
	// @r1 the radius at <C1>
	//
	// R version:
	// ```R
	// c$setR1(r1)
	// ```
	void SetR1(double r1);
	// Sets the radius at C2
	// @r2 the radius at <C2>
	//
	// R version:
	// ```R
	// c$setR2(r2)
	// ```
	void SetR2(double r2);
	// Gets the radius at C1
	//
	// R version:
	// ```R
	// c$r1()
	// ```
	//
	// @return the radius at <C1>
	inline double R1() const {
		return d_r1;
	}
	// Gets the radius at C2
	//
	// R version:
	// ```R
	// c$r2()
	// ```
	//
	// @return the radius at <C2>
	inline double R2() const {
		return d_r2;
	}

	bool Contains(const Eigen::Vector2d & point) const override;

	AABB ComputeAABB() const override;

	std::unique_ptr<Shape> Clone() const override;

	Capsule Transform(const priv::Isometry2Dd & transform) const;

	inline bool Intersects(const Capsule & other) const {
		return Intersect(d_c1,d_c2,d_r1,d_r2,
		                 other.d_c1,other.d_c2,other.d_r1,other.d_r2);
	}

	static bool Intersect(const Eigen::Vector2d & aC1,
	                      const Eigen::Vector2d & aC2,
	                      double aR1,
	                      double aR2,
	                      const Eigen::Vector2d & bC1,
	                      const Eigen::Vector2d & bC2,
	                      double bR1,
	                      double bR2);


	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
private:

	Eigen::Vector2d d_c1,d_c2;
	double d_r1,d_r2;
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
	// A pointer to a Capsule
	typedef std::shared_ptr<Polygon>       Ptr;

	// Public constructor
	// @vertices the vertices of the polygon
	//
	// R version:
	// ```R
	// p <- fmPolygonCreate(list(c(x1,y1),c(x2,y2),...))
	// ```
	Polygon(const Vector2dList & vertices);
	// virtual destructor
	virtual ~Polygon();

	// Gets the number of vertices in the polygon
	//
	// R version:
	// ```R
	// p$size()
	// ```
	//
	// @return the number of vertices in the Polygon
	size_t Size() const;

	// Gets a polygon vertex
	// @i the index of the wanted vertex in [0;<Size>-1]
	//
	// R version:
	// ```R
	// # idx still starts from 0 as in C/C++
	// p$vertex(idx)
	// ```
	//
	// @return a const reference to the wanted vertex
	const Eigen::Vector2d & Vertex(size_t i) const;

	// Sets a polygon vertex
	// @i the index of the vertex to modify in [0,<Size>-1]
	// @v the wanted value for the vertex
	//
	// R version:
	// ```R
	// # idx still starts from 0 as in C/C++
	// p$setVertex(idx,c(x,y))
	// ```
	void SetVertex(size_t i, const Eigen::Vector2d & v);


	bool Contains(const Eigen::Vector2d & point) const override;

	AABB ComputeAABB() const override;

	std::unique_ptr<Shape> Clone() const override;

private:
	Vector2dList d_vertices;
};

} // namespace myrmidon
} // namespace fort

std::ostream  & operator<<(std::ostream & out,
                           const fort::myrmidon::Capsule & c);
