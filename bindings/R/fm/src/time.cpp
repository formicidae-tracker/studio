#include "time.h"
#include "duration.h"

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


fort::myrmidon::Time * fmTime_now() {
	return new fort::myrmidon::Time(fort::myrmidon::Time::Now());
}

fort::myrmidon::Time * fmTime_parse(const std::string & date) {
	return new fort::myrmidon::Time(fort::myrmidon::Time::Parse(date));
}

fort::myrmidon::Time * fmTimeCPtr_get(const fort::myrmidon::Time::ConstPtr * t) {
	if ( t->get() == nullptr ) {
		return nullptr;
	}
	return new fort::myrmidon::Time(**t);
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

fort::myrmidon::Time::ConstPtr * fmTimeCPtr_infinite() {
	return new fort::myrmidon::Time::ConstPtr();
}

fort::myrmidon::Time::ConstPtr * fmTime_const_ptr(const fort::myrmidon::Time * t) {
	return new fort::myrmidon::Time::ConstPtr(new fort::myrmidon::Time(*t));
}




RCPP_MODULE(time) {
	Rcpp::class_<fort::myrmidon::Time>("fmTime")
		.constructor()
		.const_method("show",&fmTime_show)
		.const_method("after",&fort::myrmidon::Time::After)
		.const_method("before",&fort::myrmidon::Time::Before)
		.const_method("equals",&fort::myrmidon::Time::Equals)
		.const_method("add",&fmTime_add)
		.const_method("sub",&fmTime_sub)
		.const_method("round",&fmTime_round)
		.const_method("const_ptr",&fmTime_const_ptr)
		;

	Rcpp::class_<fort::myrmidon::Time::ConstPtr>("fmTimeCPtr")
		.const_method("show",&fmTimeCPtr_show)
		.const_method("get",&fmTimeCPtr_get)
		;


	Rcpp::function("fmTimeNow",&fmTime_now);
	Rcpp::function("fmTimeParse",&fmTime_parse);
	Rcpp::function("fmTimeInf",&fmTimeCPtr_infinite);
}
