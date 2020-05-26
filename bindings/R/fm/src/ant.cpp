//#include "identification.h"

#include "ant.h"

#include "identification.h"

#include "Rcpp.h"

void fmAnt_show(const fort::myrmidon::Ant * ant) {
	Rcpp::Rcout << "fmAnt " << ant->FormattedID() << "\n";
}

void fmCAnt_show(const fort::myrmidon::CAnt * ant) {
	Rcpp::Rcout << "fmCAnt " << ant->FormattedID() << "\n";
}

Rcpp::List fmCAnt_cIdentifications(const fort::myrmidon::CAnt * ant) {
	Rcpp::List res;
	for ( const auto & identification : ant->CIdentifications() ) {
		res.push_back(identification);
	}
	return res;
}

Rcpp::List fmAnt_cIdentifications(const fort::myrmidon::Ant * ant) {
	return fmCAnt_cIdentifications(reinterpret_cast<const fort::myrmidon::CAnt *>(ant));
}


Rcpp::List fmAnt_identifications(fort::myrmidon::Ant * ant) {
	Rcpp::List res;
	for ( const auto & identification : ant->Identifications() ) {
		res.push_back(identification);
	}
	return res;
}


RCPP_MODULE(ant) {
	Rcpp::class_<fort::myrmidon::CAnt>("fmCAnt")
		.const_method("show",&fmCAnt_show)
		.const_method("cIdentifications",&fmCAnt_cIdentifications)
		.const_method("antID",&fort::myrmidon::CAnt::AntID)
		.const_method("formattedID",&fort::myrmidon::CAnt::FormattedID)
		;

	Rcpp::class_<fort::myrmidon::Ant>("fmAnt")
		.const_method("show",&fmAnt_show)
		.const_method("cIdentifications",&fmAnt_cIdentifications)
		.method("identifications",&fmAnt_identifications)
		.const_method("antID",&fort::myrmidon::Ant::AntID)
		.const_method("formattedID",&fort::myrmidon::Ant::FormattedID)
		;

}
