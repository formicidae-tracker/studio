#include "experiment.h"

#include <Rcpp.h>

RCPP_MODULE(experiment) {
	Rcpp::class_<fort::myrmidon::CExperiment>("fmCExperiment")
		;

	Rcpp::class_<fort::myrmidon::Experiment>("fmExperiment")
		;
}
