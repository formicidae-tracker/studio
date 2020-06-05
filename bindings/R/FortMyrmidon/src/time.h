#include <fort/myrmidon/Time.hpp>

#include <RcppCommon.h>

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Time);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Time::ConstPtr);

void fmTime_show(const fort::myrmidon::Time * t);
void fmTimeCPtr_show(const fort::myrmidon::Time::ConstPtr * t);


Rcpp::Datetime fmTime_asR(const fort::myrmidon::Time & t );

fort::myrmidon::Time fmTime_fromR( SEXP exp);

fort::myrmidon::Time::ConstPtr fmTimeCPtr_fromAnySEXP(SEXP exp);
