#include <Eigen/Core>

#include "Rcpp.h"

namespace Rcpp {
template <> Eigen::Vector2d as(SEXP);
template <> SEXP wrap(const Eigen::Vector2d & v);
}
