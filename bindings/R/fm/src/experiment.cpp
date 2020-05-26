#include "experiment.h"

#include "space.h"
#include "ant.h"
#include "identification.h"
#include "time.h"

namespace Rcpp {

template <> SEXP wrap(const std::map<fort::myrmidon::Space::ID,fort::myrmidon::Space> & spaces);
template <> SEXP wrap(const std::map<fort::myrmidon::Space::ID,fort::myrmidon::CSpace> & cSpaces);

template <> SEXP wrap(const std::map<fort::myrmidon::Ant::ID,fort::myrmidon::Ant> & ants);
template <> SEXP wrap(const std::map<fort::myrmidon::Ant::ID,fort::myrmidon::CAnt> & cAnts);

template <> SEXP wrap(const std::map<fort::myrmidon::MeasurementTypeID,std::string> & mTypes);


}

RCPP_EXPOSED_ENUM_NODECL(fort::tags::Family)

#include <Rcpp.h>

void fmCExperiment_show(const fort::myrmidon::CExperiment * e) {
	Rcpp::Rcout << "fmExperiment ( Path= '"  << e->AbsoluteFilePath()
	            << "' )\n";
}


void fmExperiment_show(const fort::myrmidon::Experiment * e) {
	Rcpp::Rcout << "fmExperiment ( Path= '"  << e->AbsoluteFilePath()
	            << "' )\n";
}



RCPP_MODULE(experiment) {
	Rcpp::class_<fort::myrmidon::CExperiment>("fmCExperiment")
		.const_method("show",&fmCExperiment_show)
		.const_method("absoluteFilePath",&fort::myrmidon::CExperiment::AbsoluteFilePath)
		.const_method("cSpaces",&fort::myrmidon::CExperiment::CSpaces)
		.const_method("cAnts",&fort::myrmidon::CExperiment::CAnts)
		.const_method("freeIdentificationRangeAt",&fort::myrmidon::CExperiment::FreeIdentificationRangeAt)
		.const_method("name",&fort::myrmidon::CExperiment::Name)
		.const_method("author",&fort::myrmidon::CExperiment::Author)
		.const_method("comment",&fort::myrmidon::CExperiment::Comment)
		.const_method("family",&fort::myrmidon::CExperiment::Family)
		.const_method("defaultTagSize",&fort::myrmidon::CExperiment::DefaultTagSize)
		.const_method("threshold",&fort::myrmidon::CExperiment::Threshold)
		;

	Rcpp::class_<fort::myrmidon::Experiment>("fmExperiment")
		.const_method("show",&fmExperiment_show)
		.method("save",&fort::myrmidon::Experiment::Save)
		.const_method("absoluteFilePath",&fort::myrmidon::Experiment::AbsoluteFilePath)
		.method("createSpace",&fort::myrmidon::Experiment::CreateSpace)
		.method("deleteSpace",&fort::myrmidon::Experiment::DeleteSpace)
		.const_method("cSpaces",&fort::myrmidon::Experiment::CSpaces)
		.method("spaces",&fort::myrmidon::Experiment::Spaces)
		.method("addTrackingDataDirectory",&fort::myrmidon::Experiment::AddTrackingDataDirectory)
		.method("deleteTrackingDataDirectory",&fort::myrmidon::Experiment::DeleteTrackingDataDirectory)
		.method("createAnt",&fort::myrmidon::Experiment::CreateAnt)
		.const_method("cAnts",&fort::myrmidon::Experiment::CAnts)
		.method("ants",&fort::myrmidon::Experiment::Ants)
		.method("addIdentification",&fort::myrmidon::Experiment::AddIdentification)
		.method("deleteIdentification",&fort::myrmidon::Experiment::DeleteIdentification)
		.const_method("freeIdentificationRangeAt",&fort::myrmidon::Experiment::FreeIdentificationRangeAt)
		.const_method("name",&fort::myrmidon::Experiment::Name)
		.method("setName",&fort::myrmidon::Experiment::SetName)
		.const_method("author",&fort::myrmidon::Experiment::Author)
		.method("setAuthor",&fort::myrmidon::Experiment::SetAuthor)
		.const_method("comment",&fort::myrmidon::Experiment::Comment)
		.method("setComment",&fort::myrmidon::Experiment::SetComment)
		.const_method("family",&fort::myrmidon::Experiment::Family)
		.method("setFamily",&fort::myrmidon::Experiment::SetFamily)
		.const_method("defaultTagSize",&fort::myrmidon::Experiment::DefaultTagSize)
		.method("setDefaultTagSize",&fort::myrmidon::Experiment::SetDefaultTagSize)
		.const_method("threshold",&fort::myrmidon::Experiment::Threshold)
		.method("setThreshold",&fort::myrmidon::Experiment::SetThreshold)
		.method("createMeasurementType",&fort::myrmidon::Experiment::CreateMeasurementType)
		.method("deleteMeasurementType",&fort::myrmidon::Experiment::DeleteMeasurementType)
		.method("setMeasurementTypeName",&fort::myrmidon::Experiment::SetMeasurementTypeName)

		;


	Rcpp::function("fmExperimentOpen",&fort::myrmidon::Experiment::Open);
	Rcpp::function("fmExperimentOpenReadOnly",&fort::myrmidon::Experiment::OpenReadOnly);
	Rcpp::function("fmExperimentNewFile",&fort::myrmidon::Experiment::NewFile);
	Rcpp::function("fmExperimentCreate",&fort::myrmidon::Experiment::Create);
}


namespace Rcpp {

template <> SEXP wrap(const std::map<fort::myrmidon::Space::ID,fort::myrmidon::Space> & spaces) {
	List res;
	for ( const auto & [spaceID,space] : spaces ) {
		res[spaceID] = space;
	}
	return res;
}

template <> SEXP wrap(const std::map<fort::myrmidon::Space::ID,fort::myrmidon::CSpace> & cSpaces) {
	List res;
	for ( const auto & [spaceID,space] : cSpaces ) {
		res[spaceID] = space;
	}
	return res;
}


template <> SEXP wrap(const std::map<fort::myrmidon::Ant::ID,fort::myrmidon::Ant> & ants) {
	List res;
	for ( const auto & [antID,ant] : ants ) {
		res[ant.FormattedID()] = ant;
		res[antID] = ant;
	}
	return res;
}

template <> SEXP wrap(const std::map<fort::myrmidon::Ant::ID,fort::myrmidon::CAnt> & cAnts) {
	List res;
	for ( const auto & [antID,ant] : cAnts ) {
		res[ant.FormattedID()] = ant;
		res[antID] = ant;
	}
	return res;
}

template <> SEXP wrap(const std::map<fort::myrmidon::MeasurementTypeID,std::string> & mTypes) {
	List res;
	for ( const auto & [mType,name] : mTypes ) {
		res[mType] = name;
	}
	return res;
}


}
