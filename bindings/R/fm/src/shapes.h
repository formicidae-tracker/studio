#include <fort/myrmidon/Shapes.hpp>

#include "RcppCommon.h"

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Shape);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Circle);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Capsule);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Polygon);


void fmCircle_show(const fort::myrmidon::Circle * c);
void fmCapsule_show(const fort::myrmidon::Capsule * c);
void fmPolygon_show(const fort::myrmidon::Polygon * p);

namespace Rcpp {
template <> fort::myrmidon::Shape::ConstList as(SEXP exp);
template <> SEXP wrap(const fort::myrmidon::Shape::ConstList & list);
}
