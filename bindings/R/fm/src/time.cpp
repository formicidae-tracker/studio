#include <fort/myrmidon/Time.hpp>

#include <Rcpp.h>


// Helpers method for Duration
void fmTimeShow(const fort::myrmidon::Time * t) {
	Rcpp::Rcout << *t << "\n";
}


fort::myrmidon::Time * fmTime_now() {
	return new fort::myrmidon::Time(fort::myrmidon::Time::Now());
}

fort::myrmidon::Time * fmTime_parse(const std::string & date) {
	return new fort::myrmidon::Time(fort::myrmidon::Time::Parse(date));
}


fort::myrmidon::Time * fmTime_add(const fort::myrmidon::Time * t,
                                  fort::myrmidon::Duration & d) {
	return new fort::myrmidon::Time(t->Add(d));
}

fort::myrmidon::Time * fmTime_round(const fort::myrmidon::Time * t,
                                    fort::myrmidon::Duration & d) {
	return new fort::myrmidon::Time(t->Round(d));
}


fort::myrmidon::Duration * fmTime_sub(const fort::myrmidon::Time * a,
                                      fort::myrmidon::Time & b) {
	return new fort::myrmidon::Duration(a->Sub(b));
}



RCPP_MODULE(time) {
	Rcpp::class_<fort::myrmidon::Time>("fmTime")
		.constructor()
		.const_method("show",&fmTimeShow)
		.const_method("after",&fort::myrmidon::Time::After)
		.const_method("before",&fort::myrmidon::Time::Before)
		.const_method("equals",&fort::myrmidon::Time::Equals)
		.const_method("add",&fmTime_add)
		.const_method("sub",&fmTime_sub)
		.const_method("round",&fmTime_round)
		;

	Rcpp::function("fmTimeNow",&fmTime_now);
	Rcpp::function("fmTimeParse",&fmTime_parse);
}

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Time);
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::Duration);
