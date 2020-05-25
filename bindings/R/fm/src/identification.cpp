#include "identification.h"

#include "time.h"
#include "eigen.h"

void fmIdentification_show(const fort::myrmidon::Identification * i) {
	Rcpp::Rcout << "fmIdentification " << *i  << "\n";
}

RCPP_MODULE(identification) {
	Rcpp::class_<fort::myrmidon::Identification>("fmCIdentification")
		.const_method("show",&fmIdentification_show)
		.const_method("tagValue",&fort::myrmidon::Identification::TagValue)
		.const_method("targetAntID",&fort::myrmidon::Identification::TargetAntID)
		.const_method("start",&fort::myrmidon::Identification::Start)
		.const_method("end",&fort::myrmidon::Identification::End)
		.const_method("antPosition",&fort::myrmidon::Identification::AntPosition)
		.const_method("antAngle",&fort::myrmidon::Identification::AntAngle)
		.const_method("hasUserDefinedAntPose",&fort::myrmidon::Identification::HasUserDefinedAntPose)
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
