#include "Shape.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

Capsule::Capsule(const Eigen::Vector2d & c1,
                 const Eigen::Vector2d & c2,
                 double r1,
                 double r2)
	: d_c1(c1)
	, d_c2(c2)
	, d_r1(r1)
	, d_r2(r2) {
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

#define constraintToSegment(t,projected,point,start,startToEnd) do { \
		t = (point - start).dot(startToEnd) / startToEnd.dot(startToEnd); \
		t = clamp(t,0.0,1.0); \
		projected = t * startToEnd + start; \
	} while(0)

	Eigen::Vector2d aCC = aC2 - aC1;
	Eigen::Vector2d bCC = bC2 - bC1;

	Eigen::Vector2d bCProj,aCProj;
	double tB,tA,sumRadius;

#define intersect(startPoint) do { \
		constraintToSegment(tA,aCProj,startPoint,aC1,aCC); \
		constraintToSegment(tB,bCProj,aCProj,bC1,bCC); \
	  \
		double distSqrd = (bCProj - aCProj).squaredNorm(); \
		/*std::cerr << "dist2 is " << distSqrd << " tA " << tA << " tB " << tB <<  std::endl;*/ \
		if ( distSqrd < 1.0e-6 ) { \
			/* Segments intersects */ \
			return true; \
		} \
		sumRadius = aR1 + tA * (aR2 - aR1) + bR1 + tB * (bR2 - bR1); \
		sumRadius *= sumRadius; \
		/*std::cerr << "sumRadius " << sumRadius << " tA " << tA << " tB " << tB <<  " aR1 " << aR1 << " bR1 " << bR1 << std::endl; */ \
		if ( distSqrd <= sumRadius ) { \
			return true; \
		} \
	}while(0)

	intersect(bC1);
	intersect(bC2);
	return false;
}


} // namespace priv
} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::Capsule & c) {
	return out << "Capsule{C1:{" << c.C1().transpose() << "},R1:" << c.R1()
	           << ",C2:{" << c.C2().transpose() << "},R2:" << c.R2() << "}";

}
