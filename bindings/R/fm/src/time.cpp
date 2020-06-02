#include "time.h"
#include "duration.h"

#include "Rcpp.h"


fort::myrmidon::Time fmTime_fromR( SEXP exp) {
	double ts = Rcpp::Datetime(exp).getFractionalTimestamp();
	struct timeval tv = {
	                     .tv_sec = time_t(ts),
	                     .tv_usec = 0
	};
	tv.tv_usec = (ts - double(tv.tv_sec)) * 1e6;
	return fort::myrmidon::Time::FromTimeval(tv);
}


Rcpp::Datetime fmTime_asR(const fort::myrmidon::Time & t) {
	auto tv = t.ToTimeval();
	return Rcpp::Datetime(double(tv.tv_sec) + 1e-6 * (tv.tv_usec));
}

Rcpp::Datetime fmTime_asR_fromPtr ( const fort::myrmidon::Time * t ){
	return fmTime_asR(*t);
}


void fmTime_show(const fort::myrmidon::Time * t) {
	Rcpp::Rcout << *t << "\n";
}

void fmTimeCPtr_show(const fort::myrmidon::Time::ConstPtr * t) {
	if ( t->get() == nullptr ) {
		Rcpp::Rcout << "+/-∞\n";
		return;
	}
	Rcpp::Rcout << **t << "\n";
}


fort::myrmidon::Time fmTimeCPtr_get(const fort::myrmidon::Time::ConstPtr * t) {
	if ( t->get() == nullptr ) {
		throw std::runtime_error("time is a nullptr");
	}
	return fort::myrmidon::Time(**t);
}


fort::myrmidon::Time::ConstPtr fmTimeCPtr_infinite() {
	return fort::myrmidon::Time::ConstPtr();
}

fort::myrmidon::Time::ConstPtr fmTime_const_ptr(const fort::myrmidon::Time * t) {
	return std::make_shared<fort::myrmidon::Time>(*t);
}



RCPP_MODULE(time) {
	Rcpp::class_<fort::myrmidon::Time>("fmTime")
		.constructor()
		.const_method("show",&fmTime_show)
		.const_method("after",&fort::myrmidon::Time::After)
		.const_method("before",&fort::myrmidon::Time::Before)
		.const_method("equals",&fort::myrmidon::Time::Equals)
		.const_method("add",&fort::myrmidon::Time::Add)
		.const_method("sub",&fort::myrmidon::Time::Sub)
		.const_method("round",&fort::myrmidon::Time::Round)
		.const_method("const_ptr",&fmTime_const_ptr)
		.const_method("as.POSIXct",&fmTime_asR_fromPtr)
		;

	Rcpp::class_<fort::myrmidon::Time::ConstPtr>("fmTimeCPtr")
		.const_method("show",&fmTimeCPtr_show)
		.const_method("get",&fmTimeCPtr_get)
		;


	Rcpp::function("fmTimeNow",&fort::myrmidon::Time::Now);
	Rcpp::function("fmTimeParse",&fort::myrmidon::Time::Parse);
	Rcpp::function("fmTimeInf",&fmTimeCPtr_infinite);
}
