#include "Capsule.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

Capsule::Capsule()
	: Shape(myrmidon::Shape::Type::Capsule)
	, d_c1(0,0)
	, d_c2(0,0)
	, d_r1(0)
	, d_r2(0) {
}
Capsule::Capsule(const Eigen::Vector2d & c1,
                 const Eigen::Vector2d & c2,
                 double r1,
                 double r2)
	: Shape(myrmidon::Shape::Type::Capsule)
	, d_c1(c1)
	, d_c2(c2)
	, d_r1(r1)
	, d_r2(r2) {
}

Capsule::~Capsule() {}


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


} // namespace priv
} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::Capsule & c) {
	return out << "Capsule{C1:{" << c.C1().transpose() << "},R1:" << c.R1()
	           << ",C2:{" << c.C2().transpose() << "},R2:" << c.R2() << "}";

}
