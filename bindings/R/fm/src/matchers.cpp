#include "matchers.h"


#include "ant_static_value.h"

namespace Rcpp {
template <> std::vector<fort::myrmidon::Matcher::Ptr> as(SEXP);
}

#include <Rcpp.h>


void fmMatcher_show(const fort::myrmidon::Matcher::Ptr * m ) {
	if ( m->get() == nullptr ) {
		Rcpp::Rcout << "fmMatcher ( true )\n";
		return;
	}
	Rcpp::Rcout << "fmMatcher ( " << **m << " )\n";
}

fort::myrmidon::Matcher::Ptr fmMatcher_any() {
	return fort::myrmidon::Matcher::Ptr();
}


RCPP_MODULE(matchers) {
	Rcpp::class_<fort::myrmidon::Matcher::Ptr>("fmMatcher")
		.const_method("show",&fmMatcher_show)
		;


	Rcpp::function("fmMatcherAny",&fmMatcher_any);
	Rcpp::function("fmMatcherAnd",&fort::myrmidon::Matcher::And);
	Rcpp::function("fmMatcherOr",&fort::myrmidon::Matcher::Or);
	Rcpp::function("fmMatcherAntID",&fort::myrmidon::Matcher::AntID);
	Rcpp::function("fmMatcherAntColumn",&fort::myrmidon::Matcher::AntColumn);
	Rcpp::function("fmMatcherAntDistanceGreaterThan",&fort::myrmidon::Matcher::AntDistanceGreaterThan);
	Rcpp::function("fmMatcherAntDistanceSmallerThan",&fort::myrmidon::Matcher::AntDistanceSmallerThan);
	Rcpp::function("fmMatcherAntAngleGreaterThan",&fort::myrmidon::Matcher::AntAngleGreaterThan);
	Rcpp::function("fmMatcherAntAngleSmallerThan",&fort::myrmidon::Matcher::AntAngleSmallerThan);

}


namespace Rcpp {
template <> std::vector<fort::myrmidon::Matcher::Ptr> as(SEXP exp) {
	Rcpp::List matchers(exp);
	std::vector<fort::myrmidon::Matcher::Ptr> res;
	res.reserve(matchers.size());
	for ( const auto & m : matchers ) {
		res.push_back(as<fort::myrmidon::Matcher::Ptr>(m));
	}
	return res;
}

}
