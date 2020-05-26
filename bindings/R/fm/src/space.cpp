#include "space.h"

#include "zone.h"
#include "time.h"


namespace Rcpp {
template <> SEXP wrap(const fort::myrmidon::Zone::ByID & zones);
template <> SEXP wrap(const fort::myrmidon::Zone::ConstByID & zones);
template <> SEXP wrap(const std::pair<std::string,uint64_t> & locatedMovieFrame);
}

#include <Rcpp.h>


void fmCSpace_show(const fort::myrmidon::CSpace * s) {
	Rcpp::Rcout << "fmCSpace ( ID = " << s->SpaceID()
	            << " Name = " << s->Name()
	            << " )\n";
}

void fmSpace_show(const fort::myrmidon::Space * s) {
	Rcpp::Rcout << "fmSpace ( ID = " << s->SpaceID()
	            << " Name = " << s->Name()
	            << " )\n";
}

RCPP_MODULE(space) {
	Rcpp::class_<fort::myrmidon::CSpace>("fmCSpace")
		.const_method("show",&fmCSpace_show)
		.const_method("spaceID",&fort::myrmidon::CSpace::SpaceID)
		.const_method("name",&fort::myrmidon::CSpace::Name)
		.const_method("cZones",&fort::myrmidon::CSpace::CZones)
		.const_method("locateMovieFrame",&fort::myrmidon::CSpace::LocateMovieFrame)
		;

	Rcpp::class_<fort::myrmidon::Space>("fmSpace")
		.const_method("show",&fmSpace_show)
		.const_method("spaceID",&fort::myrmidon::Space::SpaceID)
		.const_method("name",&fort::myrmidon::Space::Name)
		.method("setName",&fort::myrmidon::Space::SetName)
		.method("createZone",&fort::myrmidon::Space::CreateZone)
		.method("deleteZone",&fort::myrmidon::Space::DeleteZone)
		.const_method("cZones",&fort::myrmidon::Space::CZones)
		.method("zones",&fort::myrmidon::Space::Zones)
		.const_method("locateMovieFrame",&fort::myrmidon::Space::LocateMovieFrame)
		;

}

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::Zone::ByID & zones) {
	List res;
	for ( const auto & [zoneID,zone] : zones) {
		res[zoneID] = zone;
	}
	return res;
}

template <> SEXP wrap(const fort::myrmidon::Zone::ConstByID & zones) {
	List res;
	for ( const auto & [zoneID,zone] : zones) {
		res[zoneID] = zone;
	}
	return res;
}

template <> SEXP wrap(const std::pair<std::string,uint64_t> & locatedMovieFrame) {
	List res;
	res["absoluteFilePath"] = locatedMovieFrame.first;
	res["movieFrameID"] = locatedMovieFrame.second;
	return res;
}


}
