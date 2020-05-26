//#include "identification.h"

#include "ant.h"

#include "Rcpp.h"

void fmAnt_show(const fort::myrmidon::Ant * ant) {
	Rcpp::Rcout << "fmAnt " << ant->FormattedID() << "\n";
}

Rcpp::List fmAnt_cIdentifications(const fort::myrmidon::Ant * ant) {
	Rcpp::List res;
	for ( const auto & identification : ant->CIdentifications() ) {
		//		res.push_back(*identification);
	}
	return res;
}



RCPP_MODULE(ant) {
	Rcpp::class_<fort::myrmidon::Ant>("fmCAnt")
		.const_method("show",&fmAnt_show)
		.const_method("cIdentifications",&fmAnt_cIdentifications)
		;

	Rcpp::class_<const fort::myrmidon::Ant>("fmAnt")
		.const_method("show",&fmAnt_show)
		.const_method("cIdentifications",&fmAnt_cIdentifications)
		;

}
