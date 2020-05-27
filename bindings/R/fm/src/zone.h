#include <fort/myrmidon/Zone.hpp>

#include <RcppCommon.h>

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Zone);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::CZone);

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::ZoneDefinition);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::CZoneDefinition);

void fmCZoneDefinition_show(const fort::myrmidon::CZoneDefinition * zd);
void fmZoneDefinition_show(const fort::myrmidon::ZoneDefinition * zd);
void fmCZone_show(const fort::myrmidon::CZone * z);
void fmZone_show(const fort::myrmidon::Zone * z);
