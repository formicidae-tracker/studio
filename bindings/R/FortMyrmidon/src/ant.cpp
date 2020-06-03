//#include "identification.h"

#include "ant.h"

#include "identification.h"
#include "color.h"
#include "ant_static_value.h"
#include "time.h"
#include "shapes.h"

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::TypedCapsuleList & l );

}


#include "Rcpp.h"

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::TypedCapsuleList & l ) {
	List capsules,shapeTypes;

	for ( const auto & [shapeType,c] : l ) {
		capsules.push_back(c);
		shapeTypes.push_back(shapeType);
	}

	List res;
	res["capsules"] = capsules;
	res["shapeTypes"] = shapeTypes;
	return res;
}

}


void fmAnt_show(const fort::myrmidon::Ant * ant) {
	Rcpp::Rcout << "fmAnt " << ant->FormattedID() << "\n";
}

void fmCAnt_show(const fort::myrmidon::CAnt * ant) {
	Rcpp::Rcout << "fmCAnt " << ant->FormattedID() << "\n";
}


RCPP_MODULE(ant) {
	Rcpp::class_<fort::myrmidon::CAnt>("fmCAnt")
		.const_method("show",&fmCAnt_show)
		.const_method("cIdentifications",&fort::myrmidon::CAnt::CIdentifications)
		.const_method("antID",&fort::myrmidon::CAnt::AntID)
		.const_method("formattedID",&fort::myrmidon::CAnt::FormattedID)
		.const_method("displayColor",&fort::myrmidon::CAnt::DisplayColor)
		.const_method("displayStatus",&fort::myrmidon::CAnt::DisplayStatus)
		.const_method("getValue",&fort::myrmidon::CAnt::GetValue)
		.const_method("capsules",&fort::myrmidon::CAnt::Capsules)
		;

	Rcpp::class_<fort::myrmidon::Ant>("fmAnt")
		.const_method("show",&fmAnt_show)
		.const_method("cIdentifications",&fort::myrmidon::Ant::CIdentifications)
		.method("identifications",&fort::myrmidon::Ant::Identifications)
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
		.const_method("capsules",&fort::myrmidon::Ant::Capsules)
		.method("deleteCapsule",&fort::myrmidon::Ant::DeleteCapsule)
		.method("clearCapsules",&fort::myrmidon::Ant::ClearCapsules)
		;

}
