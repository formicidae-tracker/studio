#include "experiment.h"

#include "space.h"
#include "ant.h"
#include "identification.h"
#include "time.h"
#include "ant_static_value.h"


RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::TrackingDataDirectoryInfo)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::SpaceDataInfo)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::ExperimentDataInfo)

namespace Rcpp {

template <> SEXP wrap(const std::map<fort::myrmidon::Space::ID,fort::myrmidon::Space> & spaces);
template <> SEXP wrap(const std::map<fort::myrmidon::Space::ID,fort::myrmidon::CSpace> & cSpaces);

template <> SEXP wrap(const std::map<fort::myrmidon::Ant::ID,fort::myrmidon::Ant> & ants);
template <> SEXP wrap(const std::map<fort::myrmidon::Ant::ID,fort::myrmidon::CAnt> & cAnts);

template <> SEXP wrap(const std::map<uint32_t,std::string> & mTypes);

template <> SEXP wrap(const std::map<std::string,std::pair<fort::myrmidon::AntMetadataType,fort::myrmidon::AntStaticValue> > & data);

template <> SEXP wrap( const std::vector<fort::myrmidon::TrackingDataDirectoryInfo> & tddInfos);

template <> SEXP wrap( const std::map<fort::myrmidon::SpaceID,fort::myrmidon::SpaceDataInfo> & spaceInfos);

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

	Rcpp::class_<fort::myrmidon::TrackingDataDirectoryInfo>("fmTrackingDataDirectoryInfo")
		.field_readonly("uri",&fort::myrmidon::TrackingDataDirectoryInfo::URI)
		.field_readonly("absoluteFilePath",&fort::myrmidon::TrackingDataDirectoryInfo::AbsoluteFilePath)
		.field_readonly("frames",&fort::myrmidon::TrackingDataDirectoryInfo::Frames)
		.field_readonly("start",&fort::myrmidon::TrackingDataDirectoryInfo::Start)
		.field_readonly("end",&fort::myrmidon::TrackingDataDirectoryInfo::End)
		;

	Rcpp::class_<fort::myrmidon::SpaceDataInfo>("fmSpaceDataInfo")
		.field_readonly("uri",&fort::myrmidon::SpaceDataInfo::URI)
		.field_readonly("name",&fort::myrmidon::SpaceDataInfo::Name)
		.field_readonly("frames",&fort::myrmidon::SpaceDataInfo::Frames)
		.field_readonly("start",&fort::myrmidon::SpaceDataInfo::Start)
		.field_readonly("end",&fort::myrmidon::SpaceDataInfo::End)
		.field_readonly("trackingDataDirectories",&fort::myrmidon::SpaceDataInfo::TrackingDataDirectories)
		;


	Rcpp::class_<fort::myrmidon::ExperimentDataInfo>("fmExperimentDataInfo")
		.field_readonly("frames",&fort::myrmidon::ExperimentDataInfo::Frames)
		.field_readonly("start",&fort::myrmidon::ExperimentDataInfo::Start)
		.field_readonly("end",&fort::myrmidon::ExperimentDataInfo::End)
		.field_readonly("spaces",&fort::myrmidon::ExperimentDataInfo::Spaces)
		;


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
		.const_method("measurementTypeNames",&fort::myrmidon::CExperiment::MeasurementTypeNames)
		.const_method("antShapeTypeNames",&fort::myrmidon::CExperiment::AntShapeTypeNames)
		.const_method("antMetadataColumns",&fort::myrmidon::CExperiment::AntMetadataColumns)
		.const_method("getDataInformations",&fort::myrmidon::CExperiment::GetDataInformations)

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
		.const_method("measurementTypeNames",&fort::myrmidon::Experiment::MeasurementTypeNames)
		.method("deleteMeasurementType",&fort::myrmidon::Experiment::DeleteMeasurementType)
		.method("setMeasurementTypeName",&fort::myrmidon::Experiment::SetMeasurementTypeName)
		.method("createAntShapeType",&fort::myrmidon::Experiment::CreateAntShapeType)
		.const_method("antShapeTypeNames",&fort::myrmidon::Experiment::AntShapeTypeNames)
		.method("setAntShapeTypeName",&fort::myrmidon::Experiment::SetAntShapeTypeName)
		.method("deleteAntShapeType",&fort::myrmidon::Experiment::DeleteAntShapeType)
		.method("addMetadataColumn",&fort::myrmidon::Experiment::AddMetadataColumn)
		.method("deleteMetadataColumn",&fort::myrmidon::Experiment::DeleteMetadataColumn)
		.const_method("antMetadataColumns",&fort::myrmidon::Experiment::AntMetadataColumns)
		.method("renameAntMetadataColumn",&fort::myrmidon::Experiment::RenameAntMetadataColumn)
		.method("setAntMetadataColumnType",&fort::myrmidon::Experiment::SetAntMetadataColumnType)
		.const_method("const",&fort::myrmidon::Experiment::Const)
		.const_method("getDataInformations",&fort::myrmidon::Experiment::GetDataInformations)
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

template <> SEXP wrap(const std::map<uint32_t,std::string> & mTypes) {
	List res;
	for ( const auto & [mType,name] : mTypes ) {
		res[mType] = name;
	}
	return res;
}

template <> SEXP wrap(const std::map<std::string,std::pair<fort::myrmidon::AntMetadataType,fort::myrmidon::AntStaticValue> > & data) {
	List res;
	for ( const auto & [columnName,pair] : data ) {
		List rPair;
		rPair["type"] = pair.first;
		rPair["value"]  = pair.second;
		res[columnName]  = rPair;
	}
	return res;
}


template <> SEXP wrap( const std::vector<fort::myrmidon::TrackingDataDirectoryInfo> & tddInfos) {
	List res;
	for ( const auto & i : tddInfos ) {
		res.push_back(i);
	}
	return res;
}

template <> SEXP wrap( const std::map<fort::myrmidon::SpaceID,fort::myrmidon::SpaceDataInfo> & spaceInfos) {
	List res;
	for ( const auto & [spaceID,info] : spaceInfos ) {
		res[spaceID] = info;
	}
	return res;
}


}
