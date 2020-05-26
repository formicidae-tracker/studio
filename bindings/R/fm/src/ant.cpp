//#include "identification.h"

#include "ant.h"

#include "identification.h"
#include "color.h"
#include "ant_static_value.h"
#include "time.h"
#include "shapes.h"

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

Rcpp::List fmCAnt_capsules(const fort::myrmidon::CAnt * ant) {
	Rcpp::List capsules,shapeTypes;

	for ( const auto & [shapeType,c] : ant->Capsules() ) {
		capsules.push_back(c);
		shapeTypes.push_back(shapeType);
	}

	Rcpp::List res;
	res["capsules"] = capsules;
	res["shapeTypes"] = shapeTypes;
	return res;
}


Rcpp::List fmAnt_capsules(const fort::myrmidon::Ant * ant) {
	return fmCAnt_capsules(reinterpret_cast<const fort::myrmidon::CAnt *>(ant));
}



RCPP_MODULE(ant) {
	Rcpp::class_<fort::myrmidon::CAnt>("fmCAnt")
		.const_method("show",&fmCAnt_show)
		.const_method("cIdentifications",&fmCAnt_cIdentifications)
		.const_method("antID",&fort::myrmidon::CAnt::AntID)
		.const_method("formattedID",&fort::myrmidon::CAnt::FormattedID)
		.const_method("displayColor",&fort::myrmidon::CAnt::DisplayColor)
		.const_method("displayStatus",&fort::myrmidon::CAnt::DisplayStatus)
		.const_method("getValue",&fort::myrmidon::CAnt::GetValue)
		.const_method("capsules",&fmCAnt_capsules)
		;

	Rcpp::class_<fort::myrmidon::Ant>("fmAnt")
		.const_method("show",&fmAnt_show)
		.const_method("cIdentifications",&fmAnt_cIdentifications)
		.method("identifications",&fmAnt_identifications)
		.const_method("antID",&fort::myrmidon::Ant::AntID)
		.const_method("formattedID",&fort::myrmidon::Ant::FormattedID)
		.const_method("displayColor",&fort::myrmidon::Ant::DisplayColor)
		.method("setDisplayColor",&fort::myrmidon::Ant::SetDisplayColor)
		.const_method("displayStatus",&fort::myrmidon::Ant::DisplayStatus)
		.method("setDisplayStatus",&fort::myrmidon::Ant::SetDisplayStatus)
		.const_method("getValue",&fort::myrmidon::Ant::GetValue)
		.method("setValue",&fort::myrmidon::Ant::SetValue)
		.method("deleteValue",&fort::myrmidon::Ant::DeleteValue)
		.method("addCapsule",&fort::myrmidon::Ant::AddCapsule)
		.const_method("capsules",&fmAnt_capsules)
		.method("deleteCapsule",&fort::myrmidon::Ant::DeleteCapsule)
		.method("clearCapsules",&fort::myrmidon::Ant::ClearCapsules)
		;

}
