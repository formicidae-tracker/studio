#include "experiment.h"

#include "space.h"
#include "ant.h"
#include "identification.h"
#include "time.h"
#include "ant_static_value.h"
#include "map.hpp"

#include <fort/myrmidon/Types.hpp>

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::TrackingDataDirectoryInfo)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::SpaceDataInfo)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::ExperimentDataInfo)

namespace Rcpp {

template <> SEXP wrap(const std::map<std::string,std::pair<fort::myrmidon::AntMetadataType,fort::myrmidon::AntStaticValue> > & data);

template <> SEXP wrap( const std::vector<fort::myrmidon::TrackingDataDirectoryInfo> & tddInfos);

}

RCPP_EXPOSED_ENUM_NODECL(fort::tags::Family)

#include <Rcpp.h>

void fmCExperiment_show(const fort::myrmidon::CExperiment * e) {
	Rcpp::Rcout << "fmCExperiment ( Path= '"  << e->AbsoluteFilePath()
	            << "' )\n";
}


void fmExperiment_show(const fort::myrmidon::Experiment * e) {
	Rcpp::Rcout << "fmExperiment ( Path= '"  << e->AbsoluteFilePath()
	            << "' )\n";
}

void fmTrackingDataDirectoryInfo_show_indent(const fort::myrmidon::TrackingDataDirectoryInfo * i,const std::string & indent) {
	Rcpp::Rcout << indent << "fmTrackingDataDirectoryInfo ([\n"
	            << indent << "  uri = '" << i->URI << "'\n"
	            << indent << "  path = '" << i->AbsoluteFilePath << "'\n"
	            << indent << "  frames = " << i->Frames << "\n"
	            << indent << "  start = " << i->Start << "\n"
	            << indent << "  end = " << i->End << "\n"
	            << indent << "])\n";
}

void fmTrackingDataDirectoryInfo_show(const fort::myrmidon::TrackingDataDirectoryInfo * i ) {
	fmTrackingDataDirectoryInfo_show_indent(i,"");
}

void fmSpaceDataInfo_show_indent(const fort::myrmidon::SpaceDataInfo * i, const std::string & indent) {
	Rcpp::Rcout << indent << "fmSpaceDataInfo ([\n"
	            << indent << "  uri = '" << i->URI << "'\n"
	            << indent << "  name = '" << i->Name << "'\n"
	            << indent << "  frames = " << i->Frames << "\n"
	            << indent << "  start = " << i->Start << "\n"
	            << indent << "  end = " << i->End << "\n"
	            << indent << "  trackingDataDirectories = ([\n";

	for ( const auto & tddi : i->TrackingDataDirectories ) {
		fmTrackingDataDirectoryInfo_show_indent(&tddi,indent + "  ");
	}
	Rcpp::Rcout << indent << "  ])\n" << indent << "])\n";
}

void fmSpaceDataInfo_show(const fort::myrmidon::SpaceDataInfo * i) {
	fmSpaceDataInfo_show_indent(i,"");
}

void fmExperimentDataInfo_show(const fort::myrmidon::ExperimentDataInfo * i) {
	Rcpp::Rcout << "fmExperimentDataInfo ([\n"
	            << "  frames = " << i->Frames << "\n"
	            << "  start = " << i->Start << "\n"
	            << "  end = "  << i->End << "\n"
	            << "  spaces = ([\n";
	for ( const auto & [spaceID,spaceInfo] : i->Spaces ) {
		fmSpaceDataInfo_show_indent(&spaceInfo,"  ");
	}
	Rcpp::Rcout << "  ])\n])\n";
}


Rcpp::DataFrame fmCExperimentWrapIdentifications(const std::map<fort::myrmidon::AntID,fort::myrmidon::TagID> & data) {
	Rcpp::NumericVector antIDs(data.size()),tagIDs(data.size());
	Rcpp::CharacterVector tagIDStr(data.size());
	size_t i = 0;
	for ( const auto & [antID,tagID] : data ) {
		antIDs[i] = antID;
		tagIDs[i] = tagID == std::numeric_limits<fort::myrmidon::TagID>::max() ? NA_INTEGER : tagID;
		tagIDStr[i] = fort::myrmidon::FormatTagID(tagID);
		++i;
	}

	auto res = Rcpp::DataFrame::create(Rcpp::_["tagDecimalValue"] = tagIDs,
	                                   Rcpp::_["antID"]  = antIDs);
	res.names() = tagIDStr;
	return res;
}

Rcpp::DataFrame fmCExperimentIdentificationsAt(const fort::myrmidon::CExperiment * e,
                                               const fort::myrmidon::Time & t,
                                               bool removeUnidentified = true) {
	return fmCExperimentWrapIdentifications(e->IdentificationsAt(t,removeUnidentified));
}

Rcpp::DataFrame fmExperimentIdentificationsAt(const fort::myrmidon::Experiment * e,
                                              const fort::myrmidon::Time & t,
                                              bool removeUnidentified = true) {
	return fmCExperimentWrapIdentifications(e->IdentificationsAt(t,removeUnidentified));
}


FM_IMPLEMENT_MAPUINT32(AntID,Ant)
FM_IMPLEMENT_MAPUINT32(AntID,CAnt)
FM_IMPLEMENT_MAPUINT32(SpaceID,CSpace)
FM_IMPLEMENT_MAPUINT32(SpaceID,Space)
FM_IMPLEMENT_MAPUINT32_WITH_DETAILS(MeasurementTypeID,string,fort::myrmidon,std,{ Rcpp::Rcout << "'" << v << "'\n"; })
FM_IMPLEMENT_MAPUINT32(SpaceID,SpaceDataInfo)


RCPP_MODULE(experiment) {
	FM_DECLARE_MAPUINT32(AntID,Ant,"fmAntByID");
	FM_DECLARE_MAPUINT32(AntID,CAnt,"fmCAntByID");
	FM_DECLARE_MAPUINT32(SpaceID,Space,"fmSpaceByID");
	FM_DECLARE_MAPUINT32(SpaceID,CSpace,"fmCSpaceByID");
	FM_DECLARE_MAPUINT32(MeasurementTypeID,string,"fmMeasurementTypeNameByID");
	FM_DECLARE_MAPUINT32(SpaceID,SpaceDataInfo,"fmSpaceDataInfoByID");

	Rcpp::class_<fort::myrmidon::TrackingDataDirectoryInfo>("fmTrackingDataDirectoryInfo")
		.const_method("show",&fmTrackingDataDirectoryInfo_show)
		.field_readonly("uri",&fort::myrmidon::TrackingDataDirectoryInfo::URI)
		.field_readonly("absoluteFilePath",&fort::myrmidon::TrackingDataDirectoryInfo::AbsoluteFilePath)
		.field_readonly("frames",&fort::myrmidon::TrackingDataDirectoryInfo::Frames)
		.field_readonly("start",&fort::myrmidon::TrackingDataDirectoryInfo::Start)
		.field_readonly("end",&fort::myrmidon::TrackingDataDirectoryInfo::End)
		;

	Rcpp::class_<fort::myrmidon::SpaceDataInfo>("fmSpaceDataInfo")
		.const_method("show",&fmSpaceDataInfo_show)
		.field_readonly("uri",&fort::myrmidon::SpaceDataInfo::URI)
		.field_readonly("name",&fort::myrmidon::SpaceDataInfo::Name)
		.field_readonly("frames",&fort::myrmidon::SpaceDataInfo::Frames)
		.field_readonly("start",&fort::myrmidon::SpaceDataInfo::Start)
		.field_readonly("end",&fort::myrmidon::SpaceDataInfo::End)
		.field_readonly("trackingDataDirectories",&fort::myrmidon::SpaceDataInfo::TrackingDataDirectories)
		;


	Rcpp::class_<fort::myrmidon::ExperimentDataInfo>("fmExperimentDataInfo")
		.const_method("show",&fmExperimentDataInfo_show)
		.field_readonly("frames",&fort::myrmidon::ExperimentDataInfo::Frames)
		.field_readonly("start",&fort::myrmidon::ExperimentDataInfo::Start)
		.field_readonly("end",&fort::myrmidon::ExperimentDataInfo::End)
		.field_readonly("spaces",&fort::myrmidon::ExperimentDataInfo::Spaces)
		;


	Rcpp::class_<fort::myrmidon::CExperiment>("fmCExperiment")
		.const_method("show",&fmCExperiment_show)
		.const_method("identificationsAt",&fmCExperimentIdentificationsAt)
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
		.const_method("identificationsAt",&fmExperimentIdentificationsAt)
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

template <> SEXP wrap(const std::map<std::string,std::pair<fort::myrmidon::AntMetadataType,fort::myrmidon::AntStaticValue> > & data) {
	List res;
	for ( const auto & [columnName,pair] : data ) {
		List rPair;
		rPair["type"] = pair.first;
		rPair["defaultValue"]  = pair.second;
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


}
