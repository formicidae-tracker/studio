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
	/**
	 * A pointer to a Shape
	 */
	typedef std::shared_ptr<Shape> Ptr;
	/**
	 *  A list of Shape
	 */
	typedef std::vector<Ptr>       List;

	/**
	 *  The type of a Shape.
	 *
	 * * Python: `py_fort_myrmidon.Shape.Type`
	 * * R : `fmShapeType` named list.
	 */
	enum class Type {
	                 /**
	                  * A Capsule
	                  *
	                  * * Python: `py_fort_myrmidon.Shape.Type.CAPSULE`
	                  * * R: `fmShapeType$CAPSULE`
	                  */
	                 CAPSULE = 0,
	                 /**
	                  * A Circle
	                  *
	                  * * Python: `py_fort_myrmidon.Shape.Type.CIRCLE`
	                  * * R: `fmShapeType$CIRCLE`
	                  */
	                 CIRCLE  = 1,
	                 /**
	                  * A Polygon
	                  *
	                  * * Python: `py_fort_myrmidon.Shape.Type.POLYGON`
	                  * * R: `fmShapeType$POLYGON`
	                  */
	                 POLYGON = 2
	};

	/**
	 *  Default destructor
	 */
	virtual ~Shape();

	/**
	 *  Gets the Shape Type
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Shape.ShapeType(self) -> py_fort_myrmidon.Shape.Type
	 * ```
	 * * R:
	 * ```R
	 * fmShapeGetType <- function(shape) # return one of the value of the named list fmShapeType
	 * ```
	 *
	 * @return the Type of the Shape
	 */
	Type ShapeType() const;

	/** \cond PRIVATE */
	virtual bool Contains(const Eigen::Vector2d & point) const = 0;

	virtual AABB ComputeAABB() const = 0;

	virtual std::unique_ptr<Shape> Clone() const = 0;

	/** \endcond PRIVATE */
protected:
	Shape(Type type);

	Type  d_type;
};

/**
 * Represent a 2D circle
 *
 */
class Circle : public Shape {
public:
	/** A pointer to a Circle */
	typedef std::shared_ptr<Circle>       Ptr;

	/**
	 *  public constructor
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Circle.init(self,center: numpy.ndarray[numpy.float64[2,1]])
	 * ```
	 * * R:
	 * ```
	 * fmCircle <- function(center = c(0,0), radius = 0 ) # returns a Rcpp_fmCircle
	 * ```
	 *
	 * @param center the center of the circle
	 * @param radius the radius of the circle
	 */
	Circle(const Eigen::Vector2d & center, double radius);

	virtual ~Circle();

	/**
	 *  Sets the center of the circle
	 *
	 * * Python: `py_fort_myrmidon.Circle.Center
	 *           (numpy.ndarray[numpy.float64[2,1]])` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCircleSetCenter <- function(circle, center = c(0.0,0.0) )
	 * ```
	 *
	 * @param center the center of the circle
	 */
	void SetCenter( const Eigen::Vector2d & center);

	/**
	 *  Gets the center of the circle
	 *
	 * * Python: `py_fort_myrmidon.Circle.Center
	 *           (numpy.ndarray[numpy.float64[2,1]])` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCircleCenter <- function(circle) # returns a numerical vector
	 * ```
	 *
	 * @return the circle center
	 */
	const Eigen::Vector2d & Center() const;


	/**
	 *  Sets the radius of the circle
	 *
	 * * Python: `py_fort_myrmidon.Circle.Radius (float)` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCircleSetRadius <- function(circle, radius = 0.0 )
	 * ```
	 *
	 * @param radius the radius of the circle
	 */
	void SetRadius(double radius);

	/**
	 *  Gets the radius of the circle
	 *
	 * * Python: `py_fort_myrmidon.Circle.Radius (float)` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCircleRadius <- function(circle) # returns a numerical
	 * ```
	 *
	 * @param radius the radius of the circle
	 */
	double Radius() const;

	/** \cond PRIVATE */

	bool Contains(const Eigen::Vector2d & point) const override;

	AABB ComputeAABB() const override;

	std::unique_ptr<Shape> Clone() const override;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
	/** \endcond PRIVATE */
private:
	Eigen::Vector2d d_center;
	double d_radius;
};

/**
 *  A capsule
 *
 * A Capsule is defined as two circle and the region in between those two circles.
 *
 * Their main purpose is to define Ant virtual body parts.
 */
class Capsule  : public Shape {
public:
	/**
	 * A pointer to a Capsule
	 */
	typedef std::shared_ptr<Capsule>       Ptr;

	/**
	 * An empty Capsule
	 */
	Capsule();

	/**
	 * Public constructor
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Capsule.__init__(self,c1: numpy.ndarray[numpy.float64[2,1]], c2:numpy.ndarray[numpy.float64[2,1]], r1: float, r2: float)
	 * ```
	 * * R:
	 * ```R
	 * fmCapsule <- function(c1 = c(0.0,0.0), c2 = c(0.0,0.0), r1 = 0.0, r2 = 0.0) # returns a Rcpp_fmCapsule
	 * ```
	 *
	 * @param c1 the first center
	 * @param c2 the second center
	 * @param r1 the radius at c1
	 * @param r2 the radius at c2
	 */
	Capsule(const Eigen::Vector2d & c1,
	        const Eigen::Vector2d & c2,
	        double r1,
	        double r2);

	virtual ~Capsule();

	/**
	 *  Sets the first circle's center
	 *
	 * * Python: `py_fort_myrmidon.Capsule.C1
	 *           (numpy.ndarray[numpy.float64[2,1]])` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCapsuleSetC1 <- function(capsule, c1 = c(0.0,0.0) )
	 * ```
	 *
	 * @param c1 the center of the first circle
	 */
	void SetC1(const Eigen::Vector2d & c1);

	/**
	 *  Sets the second circle's center
	 *
	 * * Python: `py_fort_myrmidon.Capsule.C2
	 *           (numpy.ndarray[numpy.float64[2,1]])` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCapsuleSetC2 <- function(capsule, c2 = c(0.0,0.0) )
	 * ```
	 *
	 * @param c2 the center of the second circle
	 */
	void SetC2(const Eigen::Vector2d & c2);

	/**
	 *  Gets the first circle's center
	 *
	 * * Python: `py_fort_myrmidon.Capsule.C1
	 *           (numpy.ndarray[numpy.float64[2,1]])` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCapsuleC1 <- function(capsule) # returns a numerical vector
	 * ```
	 *
	 * @return the center of the first circle
	 */
	inline const Eigen::Vector2d & C1() const {
		return d_c1;
	}

	/**
	 *  Gets the second circle's center
	 *
	 * * Python: `py_fort_myrmidon.Capsule.C2
	 *           (numpy.ndarray[numpy.float64[2,1]])` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCapsuleC2 <- function(capsule) # returns a numerical vector
	 * ```
	 *
	 * @return the center of the second circle
	 */
	inline const Eigen::Vector2d & C2() const {
		return d_c2;
	}

	/**
	 *  Sets the first circle's radius
	 *
	 * * Python: `py_fort_myrmidon.Capsule.R1 (float)` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCapsuleSetR1 <- function(capsule, r1 = 0.0 )
	 * ```
	 *
	 * @param r1 the radius of the first circle
	 */

	void SetR1(double r1);

	/**
	 *  Sets the second circle's radius
	 *
	 * * Python: `py_fort_myrmidon.Capsule.R2 (float)` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCapsuleSetR2 <- function(capsule, r2 = 0.0 )
	 * ```
	 *
	 * @param r2 the radius of the first circle
	 */
	void SetR2(double r2);

	/**
	 *  Gets the first circle's radius
	 *
	 * * Python: `py_fort_myrmidon.Capsule.R1 (float)` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCapsuleR1 <- function(capsule) # returns a numerical
	 * ```
	 *
	 * @return the radius of the first circle
	 */
	inline double R1() const {
		return d_r1;
	}

	/**
	 *  Gets the second circle's radius
	 *
	 * * Python: `py_fort_myrmidon.Capsule.R2 (float)` read-write
	 *           property of `py_fort_myrmidon.Circle` objects.
	 * * R:
	 * ```R
	 * fmCapsuleR2 <- function(capsule) # returns a numerical
	 * ```
	 *
	 * @return the radius of the second circle
	 */
	inline double R2() const {
		return d_r2;
	}

	/** \cond PRIVATE */
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
	/** \endcond PRIVATE */
private:

	Eigen::Vector2d d_c1,d_c2;
	double d_r1,d_r2;
};

/**
 * A closed polygon
 *
 * A polygon is defined by a collection of Vertex(). Polygon in
 * `fort-myrmidon` are always closed, meaning that there is no need to
 * manually close it by setting `Vertex(Size()-1) == Vertex(0)`.
 *
 * Note that order matters as {(-1,-1),(1,-1),(1,1),(-1,1)} is a
 * square, and {(-1,-1),(1,-1),(-1,1),(1,1)} is an hourglass.
 */
class Polygon  : public Shape {
public:
	/**
	 * A pointer to a Polygon
	 */
	typedef std::shared_ptr<Polygon>       Ptr;

	/**
	 * Public constructor
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Polygon.__init__(self,vertices: List[numpy.ndarray[numpy.float64[2,1]]])
	 * ```
	 * * R:
	 * ```R
	 * fmPolygon <- function(vertices = list() ) # returns a Rcpp_fmPolygon
	 * ```
	 *
	 * @param vertices the vertices of the polygon
	 */
	Polygon(const Vector2dList & vertices);


	virtual ~Polygon();

	/**
	 * Gets the number of vertices in the polygon
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Polygon.Size(self) -> int
	 * ```
	 * * R:
	 * ```R
	 * fmPolygonSize <- function(polygon) # returns an integer
	 * ```
	 *
	 * @return the number of vertices in the Polygon
	 */
	size_t Size() const;

	/**
	 * Gets a polygon vertex
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Polygon.Vertex(self,index: int) -> numpy.ndarray[numpy.float64[2,1]]
	 * ```
	 * * R:
	 * ```R
	 * fmPolygonVertex <- function(polygon, index=0) # returns a numerical vector WARNING index starts at 0
	 * ```
	 *
	 * @param i the index of the wanted vertex in [0;Size()-1]
	 *
	 * @return a const reference to the wanted vertex
	 */
	const Eigen::Vector2d & Vertex(size_t i) const;

	/**
	 * Sets a Polygon vertex
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Polygon.SetVertex(self,index: int, v: numpy.ndarray[numpy.float64[2,1]])
	 * ```
	 * * R:
	 * ```R
	 * fmPolygonVertex <- function(polygon, index=0, v = c(0.0,0.0) ) # WARNING index starts at 0
	 * ```
	 *
	 * @param i the index of the wanted vertex in [0;Size()-1]
	 * @param v the new value for the vertex
	 */
	void SetVertex(size_t i, const Eigen::Vector2d & v);

	/** \cond PRIVATE */
	bool Contains(const Eigen::Vector2d & point) const override;

	AABB ComputeAABB() const override;

	std::unique_ptr<Shape> Clone() const override;
	/** \endcond PRIVATE */
private:
	Vector2dList d_vertices;
};

} // namespace myrmidon
} // namespace fort

std::ostream  & operator<<(std::ostream & out,
                           const fort::myrmidon::Capsule & c);
