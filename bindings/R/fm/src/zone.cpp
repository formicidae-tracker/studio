#include "zone.h"
#include "shapes.h"
#include "time.h"

#include <Rcpp.h>

void fmCZoneDefinition_show(const fort::myrmidon::CZoneDefinition * zd) {
	Rcpp::Rcout << "fmZoneDefinition ([\n";
	for ( const auto & shape : zd->Geometry() ) {
		Rcpp::Rcout << "  ";
		if ( auto c = std::dynamic_pointer_cast<const fort::myrmidon::Circle>(shape) ) {
			fmCircle_show(c.get());
			continue;
		}
		if ( auto c = std::dynamic_pointer_cast<const fort::myrmidon::Capsule>(shape) ) {
			fmCapsule_show(c.get());
			continue;
		}
		if ( auto p = std::dynamic_pointer_cast<const fort::myrmidon::Polygon>(shape) ) {
			fmPolygon_show(p.get());
			continue;
		}
		Rcpp::Rcout << "unsupported shape type";
	}
	Rcpp::Rcout << "])\n";
}

void fmZoneDefinition_show(const fort::myrmidon::ZoneDefinition * zd) {
	fmCZoneDefinition_show(reinterpret_cast<const fort::myrmidon::CZoneDefinition*>(zd));
}


void fmCZone_show(const fort::myrmidon::CZone * z) {
	Rcpp::Rcout << "fmZone ( ID = " << z->ZoneID()
	            << " Name = " << z->Name()
	            << ")\n";
}

void fmZone_show(const fort::myrmidon::Zone * z) {
	fmCZone_show(reinterpret_cast<const fort::myrmidon::CZone *>(z));
}


RCPP_MODULE(zone) {
	Rcpp::class_<fort::myrmidon::CZoneDefinition>("fmCZoneDefinition")
		.const_method("show",&fmCZoneDefinition_show)
		.const_method("geometry",&fort::myrmidon::CZoneDefinition::Geometry)
		;

	Rcpp::class_<fort::myrmidon::ZoneDefinition>("fmZoneDefinition")
		.const_method("show",&fmZoneDefinition_show)
		.const_method("geometry",&fort::myrmidon::ZoneDefinition::Geometry)
		.method("setGeometry",&fort::myrmidon::ZoneDefinition::SetGeometry)
		;

	Rcpp::class_<fort::myrmidon::CZone>("fmCZone")
		.const_method("show",&fmCZone_show)
		;

	Rcpp::class_<fort::myrmidon::Zone>("fmZone")
		.const_method("show",&fmZone_show)
		;

}
