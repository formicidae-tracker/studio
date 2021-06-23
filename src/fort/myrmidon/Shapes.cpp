#include "Shapes.hpp"

#include <Eigen/Dense>

#include <fort/myrmidon/priv/Isometry2D.hpp>

#define PRIV(Class) (static_cast<priv::Class*>(d_ptr))

namespace fort {
namespace myrmidon {

Shape::Shape(Type type)
	: d_type(type) {
}

Shape::Type Shape::ShapeType() const {
	return d_type;
}

Shape::~Shape() {
}

Circle::Circle(const Eigen::Vector2d & center, double radius)
	: Shape(Type::CIRCLE)
	, d_center(center)
	, d_radius(radius) {
}

Circle::~Circle() {
}

void Circle::SetCenter( const Eigen::Vector2d & center) {
	d_center = center;
}

const Eigen::Vector2d & Circle::Center() const {
	return d_center;
}

void Circle::SetRadius(double radius) {
	d_radius = radius;
}

double Circle::Radius() const {
	return d_radius;
}

bool Circle::Contains(const Eigen::Vector2d & point) const {
	return (point - d_center).squaredNorm() <= d_radius*d_radius;
}

AABB Circle::ComputeAABB() const {
	Eigen::Vector2d r(d_radius,d_radius);
	return AABB( d_center - r, d_center + r);
}

std::unique_ptr<Shape> Circle::Clone() const {
	return std::make_unique<Circle>(*this);
}


Capsule::Capsule()
	: Shape(Type::CAPSULE)
	, d_c1(0,0)
	, d_c2(0,0)
	, d_r1(0)
	, d_r2(0) {
}

Capsule::Capsule(const Eigen::Vector2d & c1,
                 const Eigen::Vector2d & c2,
                 double r1,
                 double r2)
	: Shape(Type::CAPSULE)
	, d_c1(c1)
	, d_c2(c2)
	, d_r1(r1)
	, d_r2(r2){
}

Capsule::~Capsule() {
}

void Capsule::SetC1(const Eigen::Vector2d & c1) {
	d_c1 = c1;
}

void Capsule::SetC2(const Eigen::Vector2d & c2) {
	d_c2 = c2;
}


void Capsule::SetR1(double r1) {
	d_r1 = r1;
}

void Capsule::SetR2(double r2) {
	d_r2 = r2;
}

template <typename T>
inline T clamp(T value, T lower, T upper) {
	if ( value <= lower ) {
		return lower;
	}
	if ( value > upper ) {
		return upper;
	}
	return value;
}

bool Capsule::Intersect(const Eigen::Vector2d & aC1,
                        const Eigen::Vector2d & aC2,
                        double aR1,
                        double aR2,
                        const Eigen::Vector2d & bC1,
                        const Eigen::Vector2d & bC2,
                        double bR1,
                        double bR2) {

	// To rapidly test collision between towo capsule, we project a
	// center of a capsule on the segment of the other capsule, and
	// perform a distance check, between the radius of the center, and
	// the interpolated radius at the projected point. We repeat this
	// for the 4 capsule centers (2 per capsule).
	//
	// This is is *NOT* mathematically accurate, as we can construct
	// two capsules that should intersect, but the distance and radius
	// to be considered have to be between the projected one and one
	// of the center. In that case we won't report a collision
	// immediatly, but when the capsule will become closer.. Since we
	// are not building a physics engine, where the computation of the
	// intersection points and normals should be really accurrate, we
	// accept this approximation. Im most cases, i.e with well formed
	// capsule which are not cone - shaped, its an
	// error of about 1% of the capsule radius on the detection
	// threshold, but detection will occurs if the capsule goes closer
	// to one another. Won't affect detection of interactions.


#define constraintToSegment(t,projected,point,start,startToEnd) do { \
		t = (point - start).dot(startToEnd) / startToEnd.dot(startToEnd); \
		t = clamp(t,0.0,1.0); \
		projected = t * startToEnd + start; \
	} while(0)

	Eigen::Vector2d aCC = aC2 - aC1;
	Eigen::Vector2d bCC = bC2 - bC1;

	Eigen::Vector2d proj;
	double t,sumRadius;

#define intersect(point,startSegment,segment,pRadius1,pRadius2,radius) do {	  \
		constraintToSegment(t,proj,point,startSegment,segment); \
		double distSqrd = (proj-point).squaredNorm(); \
		/* std::cerr << "Projecting " << #point << " on " << #segment << " t: " << t << std::endl; */\
		if ( distSqrd < 1.0e-6 ) { \
			/* Segments intersects */ \
			return true; \
		} \
		sumRadius = pRadius1 + t * (pRadius2 - pRadius1) + radius; \
		sumRadius *= sumRadius; \
		/*std::cerr << "sumRadius " << sumRadius << " tA " << tA << " tB " << tB <<  " aR1 " << aR1 << " bR1 " << bR1 << std::endl; */ \
		if ( distSqrd <= sumRadius ) { \
			return true; \
		} \
	}while(0)

	intersect(bC1,aC1,aCC,aR1,aR2,bR1);
	intersect(bC2,aC1,aCC,aR1,aR2,bR2);
	intersect(aC1,bC1,bCC,bR1,bR2,aR1);
	intersect(aC2,bC1,bCC,bR1,bR2,aR2);

	return false;
}

bool Capsule::Contains(const Eigen::Vector2d & point) const {
	Eigen::Vector2d cc = d_c2 - d_c1;
	double t = (point - d_c1).dot(cc) / cc.squaredNorm();
	t = clamp(t,0.0,1.0);
	double r = d_r1 + t * (d_r2 - d_r1);
	Eigen::Vector2d diff = point - d_c1 - t * cc;
	return diff.squaredNorm() <= r*r;
}

AABB Capsule::ComputeAABB() const {
	Eigen::Vector2d r1(d_r1,d_r1),r2(d_r2,d_r2);
	AABB res(d_c1 - r1,d_c1 + r1);
	res.extend(AABB(d_c2 -r2, d_c2 + r2));
	return res;
}

Capsule Capsule::Transform(const priv::Isometry2Dd & transform) const {
	return myrmidon::Capsule(transform * d_c1,
	                         transform * d_c2,
	                         d_r1,
	                         d_r2);
}

std::unique_ptr<Shape> Capsule::Clone() const {
	return std::make_unique<Capsule>(*this);
}


Polygon::Polygon(const Vector2dList & vertices)
	: Shape(Type::POLYGON)
	, d_vertices(vertices) {
	if (d_vertices.size() < 3 ) {
		throw std::invalid_argument("A polygon needs at least 3 points");
	}
}

Polygon::~Polygon() {
}

size_t Polygon::Size() const {
	return d_vertices.size();
}

const Eigen::Vector2d & Polygon::Vertex(size_t i) const {
	return d_vertices.at(i);
}

void Polygon::SetVertex(size_t i, const Eigen::Vector2d & v) {
	d_vertices.at(i) = v;
}

AABB Polygon::ComputeAABB() const {
	AABB res(d_vertices[0],d_vertices[1]);
	for ( size_t i = 2; i < d_vertices.size(); ++i) {
		res.extend(d_vertices[i]);
	}
	return res;
}


bool Polygon::Contains(const Eigen::Vector2d & p) const {
	// O if on line, >0 if on left,  <0  if on right
#define side_criterion(start,end,point) ( (end-start).dot(Eigen::Vector2d(point.y() - start.y(),start.x() - point.x())) )

	int windingNumber = 0;
	for ( size_t i =0; i < d_vertices.size(); ++i) {
		const auto & a = d_vertices[i];
		const auto & b = d_vertices[(i+1) % d_vertices.size()];
		if ( p.y() >= a.y() ) {
			if ( p.y() >= b.y() ) {
				continue;
			}
			// p.y() < b.y()
			if ( side_criterion(a,b,p) > 0.0 ) {
				++windingNumber;
			}
		} else { // p.y() < a.y()
			if ( p.y() < b.y() ) {
				continue;
			}
			// p.y() >= b.y()
			if ( side_criterion(a,b,p) < 0.0 ) {
				--windingNumber;
			}
		}
	}

	return windingNumber != 0;
}

std::unique_ptr<Shape> Polygon::Clone() const {
	return std::make_unique<Polygon>(*this);
}

} // namespace myrmidon
} // namespace fort

std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Capsule & c) {
	return out << "Capsule{C1:{" << c.C1().transpose() << "},R1:" << c.R1()
	           << ",C2:{" << c.C2().transpose() << "},R2:" << c.R2() << "}";
}
