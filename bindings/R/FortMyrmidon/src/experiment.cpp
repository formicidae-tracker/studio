#include "experiment.h"

#include "space.h"
#include "ant.h"
#include "identification.h"
#include "time.h"
#include "ant_static_value.h"

#include <fort/myrmidon/Types.hpp>

RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::TrackingDataDirectoryInfo)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::SpaceDataInfo)
RCPP_EXPOSED_CLASS_NODECL(fort::myrmidon::ExperimentDataInfo)

namespace Rcpp {

template <> SEXP wrap(const std::map<std::string,std::pair<fort::myrmidon::AntMetadataType,fort::myrmidon::AntStaticValue> > & data);


template <> SEXP wrap( const std::map<fort::myrmidon::AntID,fort::myrmidon::Ant> & ants);

template <> SEXP wrap( const std::map<fort::myrmidon::AntID,fort::myrmidon::CAnt> & ants );

template <> SEXP wrap( const std::map<fort::myrmidon::SpaceID,fort::myrmidon::Space> & spaces);

template <> SEXP wrap( const std::map<fort::myrmidon::SpaceID,fort::myrmidon::CSpace> & spaces);

template <> SEXP wrap( const std::map<fort::myrmidon::MeasurementTypeID,std::string> & types);

template <> SEXP wrap( const fort::myrmidon::ExperimentDataInfo & infos);

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
	res.attr("row.names") = tagIDStr;
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




RCPP_MODULE(experiment) {

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


template <> SEXP wrap( const std::map<fort::myrmidon::AntID,fort::myrmidon::Ant> & ants) {
	CharacterVector formattedID(ants.size());
	IntegerVector antIDs(ants.size());
	List objects(ants.size()),res;
	size_t i = 0;
	for ( const auto & [antID,ant] : ants ) {
		formattedID[i] = ant.FormattedID();
		antIDs[i] = antID;
		objects[i] = ant;
		i++;
	}
	res["summary"] = DataFrame::create(_["antID"]  = antIDs,
	                                   _["formattedID"]  = formattedID);
	res["objects"] = objects;
	return res;
}

template <> SEXP wrap( const std::map<fort::myrmidon::AntID,fort::myrmidon::CAnt> & ants ) {
	CharacterVector formattedID(ants.size());
	IntegerVector antIDs(ants.size());
	List objects(ants.size()),res;
	size_t i = 0;
	for ( const auto & [antID,ant] : ants ) {
		formattedID[i] = ant.FormattedID();
		antIDs[i] = antID;
		objects[i] = ant;
		i++;
	}
	res["summary"] = DataFrame::create(_["antID"]  = antIDs,
	                                   _["formattedID"]  = formattedID);
	res["objects"] = objects;
	return res;
}

template <> SEXP wrap( const std::map<fort::myrmidon::SpaceID,fort::myrmidon::Space> & spaces) {
	CharacterVector names(spaces.size());
	IntegerVector spaceIDs(spaces.size());
	List objects(spaces.size()),res;
	size_t i = 0;
	for ( const auto & [spaceID,space] : spaces ) {
		names[i] = space.Name();
		spaceIDs[i] = spaceID;
		objects[i] = space;
		i++;
	}
	res["summary"] = DataFrame::create(_["spaceID"]  = spaceIDs,
	                                   _["name"]  = names);
	res["objects"] = objects;
	return res;
}

template <> SEXP wrap( const std::map<fort::myrmidon::SpaceID,fort::myrmidon::CSpace> & spaces)  {
	CharacterVector names(spaces.size());
	IntegerVector spaceIDs(spaces.size());
	List objects(spaces.size()),res;
	size_t i = 0;
	for ( const auto & [spaceID,space] : spaces ) {
		names[i] = space.Name();
		spaceIDs[i] = spaceID;
		objects[i] = space;
		i++;
	}
	res["summary"] = DataFrame::create(_["spaceID"]  = spaceIDs,
	                                   _["name"]  = names);
	res["objects"] = objects;
	return res;
}

template <> SEXP wrap( const std::map<fort::myrmidon::MeasurementTypeID,std::string> & types) {
	IntegerVector IDs(types.size());
	CharacterVector names(types.size());
	size_t i = 0;
	for ( const auto & [typeID,name] : types ) {
		IDs[i] = typeID;
		names[i] = name;
		++i;
	}
	return DataFrame::create(_["typeID"] = IDs,
	                         _["name"] = names);
}

template <> SEXP wrap( const fort::myrmidon::ExperimentDataInfo & infos) {
	CharacterVector tddURIs,tddPaths,spaceURIs,spaceNames;
	IntegerVector tddFrames,spaceIDs,spaceFrames;
	DatetimeVector tddStart(0),tddEnd(0),spaceStart(0),spaceEnd(0);


	for ( const auto & [spaceID,space] : infos.Spaces) {
		for ( const auto & tdd : space.TrackingDataDirectories ) {
			tddURIs.push_back(tdd.URI);
			tddPaths.push_back(tdd.AbsoluteFilePath);
			tddFrames.push_back(tdd.Frames);
			tddStart.push_back(fmTime_asR(tdd.Start));
			tddEnd.push_back(fmTime_asR(tdd.End));

			spaceIDs.push_back(spaceID);
			spaceURIs.push_back(space.URI);
			spaceNames.push_back(space.Name);
			spaceFrames.push_back(space.Frames);
			spaceStart.push_back(fmTime_asR(space.Start));
			spaceEnd.push_back(fmTime_asR(space.End));
		}
	}
	spaceStart.attr("class") = "POSIXct";
	spaceEnd.attr("class") = "POSIXct";
	tddStart.attr("class") = "POSIXct";
	tddEnd.attr("class") = "POSIXct";
	List res;
	res["frames"] = infos.Frames;
	res["start"]  = fmTime_asR(infos.Start);
	res["end"] = fmTime_asR(infos.End);
	res["details"] = DataFrame::create(_["space.ID"]     = spaceIDs,
	                                   _["space.name"]   = spaceNames,
	                                   _["space.frames"] = spaceFrames,
	                                   _["space.start"]  = spaceStart,
	                                   _["space.end"]    = spaceEnd,
	                                   _["tdd.URI"]      = tddURIs,
	                                   _["tdd.path"]     = tddPaths,
	                                   _["tdd.frames"]   = tddFrames,
	                                   _["tdd.start"]    = tddStart,
	                                   _["tdd.end"]      = tddEnd);
	return res;
}



}
