#include "identification.h"

#include "time.h"
#include "eigen.h"

#include "Rcpp.h"


void fmIdentification_show(const fort::myrmidon::Identification * i) {
	Rcpp::Rcout << "fmIdentification " << *i  << "\n";
}

void fmCIdentification_show(const fort::myrmidon::CIdentification * i) {
	fmIdentification_show(reinterpret_cast<const fort::myrmidon::Identification*>(i));
}


RCPP_MODULE(identification) {
	Rcpp::class_<fort::myrmidon::CIdentification>("fmCIdentification")
		.const_method("show",&fmCIdentification_show)
		.const_method("tagValue",&fort::myrmidon::CIdentification::TagValue)
		.const_method("targetAntID",&fort::myrmidon::CIdentification::TargetAntID)
		.const_method("start",&fort::myrmidon::CIdentification::Start)
		.const_method("end",&fort::myrmidon::CIdentification::End)
		.const_method("antPosition",&fort::myrmidon::CIdentification::AntPosition)
		.const_method("antAngle",&fort::myrmidon::CIdentification::AntAngle)
		.const_method("hasUserDefinedAntPose",&fort::myrmidon::CIdentification::HasUserDefinedAntPose)
		;

	Rcpp::class_<fort::myrmidon::Identification>("fmIdentification")
		.const_method("show",&fmIdentification_show)
		.const_method("tagValue",&fort::myrmidon::Identification::TagValue)
		.const_method("targetAntID",&fort::myrmidon::Identification::TargetAntID)
		.method("setStart",&fort::myrmidon::Identification::SetStart)
		.method("setEnd",&fort::myrmidon::Identification::SetEnd)
		.const_method("start",&fort::myrmidon::Identification::Start)
		.const_method("end",&fort::myrmidon::Identification::End)
		.const_method("antPosition",&fort::myrmidon::Identification::AntPosition)
		.const_method("antAngle",&fort::myrmidon::Identification::AntAngle)
		.const_method("hasUserDefinedAntPose",&fort::myrmidon::Identification::HasUserDefinedAntPose)
		.method("setUserDefinedAntPose",&fort::myrmidon::Identification::SetUserDefinedAntPose)
		.method("clearUserDefinedAntPose",&fort::myrmidon::Identification::ClearUserDefinedAntPose)
		;
}
