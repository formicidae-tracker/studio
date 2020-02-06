#include "Shape.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

Capsule::Capsule(const Eigen::Vector2d & a,
                 const Eigen::Vector2d & b,
                 double aRadius,
                 double bRadius)
	: d_a(a)
	, d_b(b)
	, d_ra(aRadius)
	, d_rb(bRadius) {
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
