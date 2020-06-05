#include <fort/myrmidon/Query.hpp>


#include <RcppCommon.h>

namespace Rcpp {
template <>  fort::myrmidon::IdentifiedFrame::ConstPtr as(SEXP);
template <>  fort::myrmidon::CollisionFrame::ConstPtr as(SEXP);
template <>  fort::myrmidon::AntTrajectory::ConstPtr as(SEXP);
template <>  fort::myrmidon::AntInteraction::ConstPtr as(SEXP);


template <> SEXP wrap(const fort::myrmidon::IdentifiedFrame::ConstPtr & f);
template <> SEXP wrap(const fort::myrmidon::CollisionFrame::ConstPtr & f);
template <> SEXP wrap(const fort::myrmidon::AntTrajectory::ConstPtr & at);
template <> SEXP wrap(const fort::myrmidon::AntInteraction::ConstPtr & ai);
}
