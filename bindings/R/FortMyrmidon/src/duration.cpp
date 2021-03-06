#include "duration.h"

#include "Rcpp.h"



// Helpers method for Duration
void fmDuration_show(fort::myrmidon::Duration * d) {
	Rcpp::Rcout << *d << "\n";
}

fort::myrmidon::Duration fmHour(double v) {
	return fort::myrmidon::Duration(v * fort::myrmidon::Duration::Hour.Nanoseconds());
}

fort::myrmidon::Duration fmMinute(double v) {
	return fort::myrmidon::Duration(v * fort::myrmidon::Duration::Minute.Nanoseconds());
}

fort::myrmidon::Duration fmSecond(double v) {
	return fort::myrmidon::Duration(v * fort::myrmidon::Duration::Second.Nanoseconds());
}

fort::myrmidon::Duration fmMillisecond(double v) {
	return fort::myrmidon::Duration(v * fort::myrmidon::Duration::Millisecond.Nanoseconds());
}

fort::myrmidon::Duration fmMicrosecond(double v) {
	return fort::myrmidon::Duration(v * fort::myrmidon::Duration::Microsecond.Nanoseconds());
}

fort::myrmidon::Duration fmNanosecond(int64_t v) {
	return fort::myrmidon::Duration(v);
}


RCPP_MODULE(duration) {
	Rcpp::class_<fort::myrmidon::Duration>("fmDuration")
		.constructor()
		.constructor<int64_t>()
		.method("hours",
		        &fort::myrmidon::Duration::Hours)
		.method("minutes",
		        &fort::myrmidon::Duration::Minutes)
		.method("seconds",
		        &fort::myrmidon::Duration::Seconds)
		.method("milliseconds",
		        &fort::myrmidon::Duration::Milliseconds)
		.method("microseconds",
		        &fort::myrmidon::Duration::Microseconds)
		.method("nanoseconds",
		        &fort::myrmidon::Duration::Nanoseconds)
		.method("show",&fmDuration_show)
		;
	Rcpp::function("fmHour",&fmHour);
	Rcpp::function("fmMinute",&fmMinute);
	Rcpp::function("fmSecond",&fmSecond);
	Rcpp::function("fmMillisecond",&fmMillisecond);
	Rcpp::function("fmMicrosecond",&fmMicrosecond);
	Rcpp::function("fmNanosecond",&fmNanosecond);
}
