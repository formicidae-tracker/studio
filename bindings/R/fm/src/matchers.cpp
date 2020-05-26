#include "matchers.h"


#include <Rcpp.h>

RCPP_MODULE(matchers) {
	Rcpp::class_<fort::myrmidon::Matcher::Ptr>("fmMatcher")
		;
}
