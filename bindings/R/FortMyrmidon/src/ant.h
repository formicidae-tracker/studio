#include <fort/myrmidon/Ant.hpp>

#include "RcppCommon.h"

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Ant)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::CAnt)
RCPP_EXPOSED_ENUM_NODECL(fort::myrmidon::Ant::DisplayState)

void fmAnt_show(const fort::myrmidon::Ant * ant);
void fmCAnt_show(const fort::myrmidon::CAnt * ant);
