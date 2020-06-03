#include <fort/myrmidon/Experiment.hpp>


#include <RcppCommon.h>

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Experiment)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::CExperiment)


void fmCExperiment_show(const fort::myrmidon::CExperiment * e);
void fmExperiment_show(const fort::myrmidon::Experiment * e);
