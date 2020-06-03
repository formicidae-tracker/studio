#include <fort/myrmidon/Identification.hpp>

#include "RcppCommon.h"

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Identification)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::CIdentification)

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::Identification::List &);
template <> SEXP wrap(const fort::myrmidon::Identification::ConstList &);

}

void fmIdentification_show(const fort::myrmidon::Identification * i);

void fmCIdentification_show(const fort::myrmidon::CIdentification * i);
