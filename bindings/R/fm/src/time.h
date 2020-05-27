#include <fort/myrmidon/Time.hpp>

#include <RcppCommon.h>

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Time);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Time::ConstPtr);

void fmTime_show(const fort::myrmidon::Time * t);
void fmTimeCPtr_show(const fort::myrmidon::Time::ConstPtr * t);
