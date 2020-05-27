#include "zone.h"
#include "shapes.h"
#include "time.h"

namespace Rcpp {
template <> SEXP wrap(const fort::myrmidon::ZoneDefinition::ConstList & );
template <> SEXP wrap(const fort::myrmidon::ZoneDefinition::List & );
}

#include <Rcpp.h>


#define ZONE_DEFINITION_SHOW(ClassName,zd) do {	  \
	Rcpp::Rcout << #ClassName " ([\n"; \
	for ( const auto & shape : zd->Geometry() ) { \
		Rcpp::Rcout << "  "; \
		if ( auto c = std::dynamic_pointer_cast<const fort::myrmidon::Circle>(shape) ) { \
			fmCircle_show(c.get()); \
			continue; \
		} \
		if ( auto c = std::dynamic_pointer_cast<const fort::myrmidon::Capsule>(shape) ) { \
			fmCapsule_show(c.get()); \
			continue; \
		} \
		if ( auto p = std::dynamic_pointer_cast<const fort::myrmidon::Polygon>(shape) ) { \
			fmPolygon_show(p.get()); \
			continue; \
		} \
		Rcpp::Rcout << "unsupported shape type"; \
	} \
	Rcpp::Rcout << "])\n"; \
	}while(0)

void fmCZoneDefinition_show(const fort::myrmidon::CZoneDefinition * zd) {
	ZONE_DEFINITION_SHOW(fmCZoneDefinition,zd);
}

void fmZoneDefinition_show(const fort::myrmidon::ZoneDefinition * zd) {
	ZONE_DEFINITION_SHOW(fmZoneDefinition,zd);
}

void fmCZone_show(const fort::myrmidon::CZone * z) {
	Rcpp::Rcout << "fmCZone ( ID = " << z->ZoneID()
	            << " Name = " << z->Name()
	            << ")\n";
}

void fmZone_show(const fort::myrmidon::Zone * z) {
	Rcpp::Rcout << "fmZone ( ID = " << z->ZoneID()
	            << " Name = " << z->Name()
	            << ")\n";
}


RCPP_MODULE(zone) {

	Rcpp::class_<fort::myrmidon::CZoneDefinition>("fmCZoneDefinition")
		.const_method("show",&fmCZoneDefinition_show)
		.const_method("geometry",&fort::myrmidon::CZoneDefinition::Geometry)
		.const_method("start",&fort::myrmidon::CZoneDefinition::Start)
		.const_method("end",&fort::myrmidon::CZoneDefinition::End)
		;

	Rcpp::class_<fort::myrmidon::ZoneDefinition>("fmZoneDefinition")
		.const_method("show",&fmZoneDefinition_show)
		.const_method("geometry",&fort::myrmidon::ZoneDefinition::Geometry)
		.method("setGeometry",&fort::myrmidon::ZoneDefinition::SetGeometry)
		.const_method("start",&fort::myrmidon::ZoneDefinition::Start)
		.const_method("end",&fort::myrmidon::ZoneDefinition::End)
		.method("setStart",&fort::myrmidon::ZoneDefinition::SetStart)
		.method("setEnd",&fort::myrmidon::ZoneDefinition::SetEnd)
		;

	Rcpp::class_<fort::myrmidon::CZone>("fmCZone")
		.const_method("show",&fmCZone_show)
		.const_method("cDefinitions",&fort::myrmidon::CZone::CDefinitions)
		.const_method("name",&fort::myrmidon::CZone::Name)
		.const_method("zoneID",&fort::myrmidon::CZone::ZoneID)
		;

	Rcpp::class_<fort::myrmidon::Zone>("fmZone")
		.const_method("show",&fmZone_show)
		.method("addDefinition",&fort::myrmidon::Zone::AddDefinition)
		.const_method("cDefinitions",&fort::myrmidon::Zone::CDefinitions)
		.method("definitions",&fort::myrmidon::Zone::Definitions)
		.method("eraseDefinition",&fort::myrmidon::Zone::EraseDefinition)
		.const_method("name",&fort::myrmidon::Zone::Name)
		.method("setName",&fort::myrmidon::Zone::SetName)
		.const_method("zoneID",&fort::myrmidon::Zone::ZoneID)
		;

}

namespace Rcpp {

template <> SEXP wrap(const fort::myrmidon::ZoneDefinition::ConstList & l) {
	List res;
	for ( const auto & zd : l ) {
		res.push_back(zd);
	}
	return res;
}

template <> SEXP wrap(const fort::myrmidon::ZoneDefinition::List & l ) {
	List res;
	for ( const auto & zd : l ) {
		res.push_back(zd);
	}
	return res;
}

}
