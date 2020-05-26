#include <fort/myrmidon/Types.hpp>

#include "RcppCommon.h"

namespace Rcpp {
template <> Eigen::Vector2d as(SEXP);
template <> SEXP wrap(const Eigen::Vector2d & v);

template <> fort::myrmidon::Vector2dList as(SEXP);
template <> SEXP wrap(const fort::myrmidon::Vector2dList & v);

}
