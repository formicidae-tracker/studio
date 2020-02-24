#include "Polygon.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

Polygon::Polygon( const Vector2dList & vertices)
	: Shape(Shape::Type::Polygon)
	, d_vertices(vertices) {
	if (d_vertices.size() < 3 ) {
		throw std::invalid_argument("A polygon needs at leas 3 points");
	}
}

Polygon::~Polygon() {
}

size_t Polygon::Size() const {
	return d_vertices.size();
}

const Eigen::Vector2d & Polygon::Vertex(size_t i) const {
	if ( i >= d_vertices.size() ) {
		throw std::invalid_argument( "Index "
		                             + std::to_string(i)
		                             + " is out of range [0,"
		                             + std::to_string(d_vertices.size()) + "[");
	}
	return d_vertices[i];
}

void Polygon::SetVertex(size_t i, const Eigen::Vector2d & v) {
	if ( i >= d_vertices.size() ) {
		throw std::invalid_argument( "Index "
		                             + std::to_string(i)
		                             + " is out of range [0,"
		                             + std::to_string(d_vertices.size()) + "[");
	}
	d_vertices[i] = v;
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

} // namespace priv
} // namespace myrmidon
} // namespace fort
