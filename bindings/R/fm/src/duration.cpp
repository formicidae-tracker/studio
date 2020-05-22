#include <fort/myrmidon/Time.hpp>

#include <RcppCommon.h>

namespace Rcpp {
template <> SEXP wrap(const fort::myrmidon::Duration &);
template <> fort::myrmidon::Duration as(SEXP);
}

#include <Rcpp.h>

// Helpers method for Duration
void fmDuration_to_string(fort::myrmidon::Duration * d) {
	Rcpp::Rcout << *d << "\n";
}


RCPP_MODULE(duration) {
	Rcpp::class_<fort::myrmidon::Duration>("fmDuration")
		.constructor()
		.constructor<int64_t>()
		.method("hours",&fort::myrmidon::Duration::Hours)
		.method("minutes",&fort::myrmidon::Duration::Minutes)
		.method("seconds",&fort::myrmidon::Duration::Seconds)
		.method("milliseconds",&fort::myrmidon::Duration::Milliseconds)
		.method("microseconds",&fort::myrmidon::Duration::Microseconds)
		.method("nanoseconds",&fort::myrmidon::Duration::Nanoseconds)
		.method("show",&fmDuration_to_string)
		;
}
