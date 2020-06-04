#' Enum list for \code{fort::myrmidon::AntMetadataType}
#' @description list that represent the
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon/fort::myrmidon::AntMetadataType}{fort::myrmidon::AntMetadataType}
#'     enum
#' @usage names(which( mdTypeValue == fmAntMetadataType) )
fmAntMetadataType <- list ( BOOL = 0L, INT = 1L, DOUBLE = 2L, STRING = 3L, TIME = 4L)


#' @name fmAntByID
#' @title a \code{std::map} of \code{\link{fmAnt}} by their ID
#' @description a \code{std::map} of \code{\link{fmAnt}} by their
#'     ID. It looks like an R list, but the \code{[[} R operator will
#'     only report a value if the corresponding ID is in the list, and
#'     fails otherwise.
#' @field as.list returns the map as an actual list (to iterate over)
#' @field size returns the size of the list
#' @examples
#' \dontrun{
#' antByID[[42]] # gets the fmAnt with ID 42. May fail if there are no fmAnt with ID 42
#' antByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
#' }
NULL

#' @name fmCAntByID
#' @title a \code{std::map} of \code{\link{fmCAnt}} by their ID
#' @description a \code{std::map} of \code{\link{fmCAnt}} by their
#'     ID. It looks like an R list, but the \code{[[} R operator will
#'     only report a value if the corresponding ID is in the list, and
#'     fails otherwise.
#' @field as.list returns the map as an actual list (to iterate over)
#' @field size returns the size of the list
#' @examples
#' \dontrun{
#' cantByID[[42]] # gets the fmCAnt with ID 42. May fail if there are no fmCAnt with ID 42
#' cantByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
#' }
NULL

#' @name fmSpaceByID
#' @title a \code{std::map} of \code{\link{fmSpace}} by their ID
#' @description a \code{std::map} of \code{\link{fmSpace}} by their
#'     ID. It looks like an R list, but the \code{[[} R operator will
#'     only report a value if the corresponding ID is in the list, and
#'     fails otherwise.
#' @field as.list returns the map as an actual list (to iterate over)
#' @field size returns the size of the list
#' @examples
#' \dontrun{
#' spaceByID[[42]] # gets the fmSpace with ID 42. May fail if there are no fmSpace with ID 42
#' spaceByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
#' }
NULL

#' @name fmCSpaceByID
#' @title a \code{std::map} of \code{\link{fmCSpace}} by their ID
#' @description a \code{std::map} of \code{\link{fmCSpace}} by their
#'     ID. It looks like an R list, but the \code{[[} R operator will
#'     only report a value if the corresponding ID is in the list, and
#'     fails otherwise.
#' @field as.list returns the map as an actual list (to iterate over)
#' @field size returns the size of the list
#' @examples
#' \dontrun{
#' antByID[[42]] # gets the fmCSpace with ID 42. May fail if there are no fmCSpace with ID 42
#' antByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
#' }
NULL

#' @name fmMeasurementTypeNameByID
#' @title a \code{std::map} of string representing measuremnt type by their ID
#' @description a \code{std::map} of string by their
#'     ID. It looks like an R list, but the \code{[[} R operator will
#'     only report a value if the corresponding ID is in the list, and
#'     fails otherwise.
#' @field as.list returns the map as an actual list (to iterate over)
#' @field size returns the size of the list
#' @examples
#' \dontrun{
#' antByID[[42]] # gets the name of the measurement type with ID 42. May fail if there are no measurement type with ID 42
#' antByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
#' }
NULL

#' @name fmSpaceDataInfoByID
#' @title a \code{std::map} of \code{\link{fmSpaceDataInfo}} by their ID
#' @description a \code{std::map} of \code{\link{fmSpaceDataInfo}} by their
#'     ID. It looks like an R list, but the \code{[[} R operator will
#'     only report a value if the corresponding ID is in the list, and
#'     fails otherwise.
#' @field as.list returns the map as an actual list (to iterate over)
#' @field size returns the size of the list
#' @examples
#' \dontrun{
#' antByID[[42]] # gets the fmSpaceDataInfo with ID 42. May fail if there are no fmSpaceDataInfo with ID 42
#' antByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
#' }
NULL


#' @name fmTrackingDataDirectoryInfo
#' @title a reference class that summarizes information about an
#'     output directory of FORT
#' @description wrapper for
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::TrackingDataDirectoryInfo/fort::myrmidon::TrackingDataDirectoryInfo}{fort::myrmidon::TrackingDataDirectoryInfo}
#'
#' @field uri the URI used in FORT studio
#' @field absoluteFilePath actual location of the directory on the filesystem
#' @field frames the number of stored frame in the directory
#' @field start the first captured \code{\link{fmTime}} in the directory
#' @field end the last captured \code{\link{fmTime}} in the directory
NULL

#' @name fmSpaceDataInfo
#' @title a reference class that summarizes data information about a
#'     \code{\link{fmSpace}} in the \code{\link{fmExperiment}}
#' @description wrapper for
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::SpaceDataInfo/fort::myrmidon::SpaceDataInfo}{fort::myrmidon::SpaceDataInfo}
#'
#' @field uri the URI used in FORT studio
#' @field name the user defined name of the space
#' @field frames the number of frame accessible in the space
#' @field start the first captured \code{\link{fmTime}} in the space
#' @field end the last captured \code{\link{fmTime}} in the space
#' @field trackingDataDirectories a list of \code{\link{fmTrackingDataDirectoryInfo}}
NULL

#' @name fmExperimentDataInfo
#' @title a reference class that summarizes data information about a \code{\link{fmExperiment}}
#' @description wrapper for
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::ExperimentDataInfo/fort::myrmidon::SpaceDataInfo}{fort::myrmidon::ExperimentDataInfo}
#'
#' @field frames the number of frame accessible in the space
#' @field start the first captured \code{\link{fmTime}} in the experiment
#' @field end the last captured \code{\link{fmTime}} in the experiment
#' @field spaces a \code{\link{fmSpaceDataInfoByID}}, a map of all \code{\link{fmSpaceDataInfo}} by their corresponding space \code{fmSpace$spaceID()}
NULL

#' @name fmExperiment
#' @title a reference class wrapper for the C++
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment}{fort::myrmidon::Experiment}
#'     class.
#' @field const return a const version, i.e. \code{\link{fmCExperiment}} for this fmExperiment
#' @field save
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::Save}{fort::myrmidon::Experiment::Save}
#' @field absoluteFilePath
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::AbsoluteFilePath}{fort::myrmidon::Experiment::AbsoluteFilePath}
#' @field createSpace
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::CreateSpace}{fort::myrmidon::Experiment::CreateSpace}
#' @field deleteSpace
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::DeleteSpace}{fort::myrmidon::Experiment::DeleteSpace}
#' @field cSpaces returns a \code{\link{fmCSpaceByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::CSpaces}{fort::myrmidon::Experiment::CSpaces}
#' @field spaces returns a \code{\link{fmSpaceByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::Spaces}{fort::myrmidon::Experiment::Spaces}
#' @field addTrackingDataDirectory
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::AddTrackingDataDirectory}{fort::myrmidon::Experiment::AddTrackingDataDirectory}
#' @field deleteTrackingDataDirectory
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::DeleteTrackingDataDirectory}{fort::myrmidon::Experiment::DeleteTrackingDataDirectory}
#' @field createAnt
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::CreateAnt}{fort::myrmidon::Experiment::CreateAnt}
#' @field cAnts returns a \code{\link{fmCAntByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::CAnts}{fort::myrmidon::Experiment::CAnts}
#' @field ants returns a \code{\link{fmAntByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::Ants}{fort::myrmidon::Experiment::Ants}
#' @field addIdentification
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::AddIdentification}{fort::myrmidon::Experiment::AddIdentification}
#' @field deleteIdentification
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::DeleteIdentification}{fort::myrmidon::Experiment::DeleteIdentification}
#' @field freeIdentificationRangeAt
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::FreeIdentificationRangeAt}{fort::myrmidon::Experiment::FreeIdentificationRangeAt}
#' @field name
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::Name}{fort::myrmidon::Experiment::Name}
#' @field setName
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetName}{fort::myrmidon::Experiment::SetName}
#' @field comment
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::Comment}{fort::myrmidon::Experiment::Comment}
#' @field setComment
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetComment}{fort::myrmidon::Experiment::SetComment}
#' @field author
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::Author}{fort::myrmidon::Experiment::Author}
#' @field setAuthor
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetAuthor}{fort::myrmidon::Experiment::SetAuthor}
#' @field family
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::Family}{fort::myrmidon::Experiment::Family}
#' @field setFamily
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetFamily}{fort::myrmidon::Experiment::SetFamily}
#' @field defaultTagSize
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::DefaultTagSize}{fort::myrmidon::Experiment::DefaultTagSize}
#' @field setDefaultTagSize
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetDefaultTagSize}{fort::myrmidon::Experiment::SetDefaultTagSize}
#' @field threshold
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::Threshold}{fort::myrmidon::Experiment::Threshold}
#' @field setThreshold
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetThreshold}{fort::myrmidon::Experiment::SetThreshold}
#' @field createMeasurementType
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::CreateMeasurementType}{fort::myrmidon::Experiment::CreateMeasurementType}
#' @field measurementTypeNames returns a \code{\link{fmMeasurementTypeNameByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::MeasurementTypeNames}{fort::myrmidon::Experiment::MeasurementTypeNames}
#' @field deleteMeasurementType
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::DeleteMeasurementType}{fort::myrmidon::Experiment::DeleteMeasurementType}
#' @field setMeasurementTypeName
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetMeasurementTypeName}{fort::myrmidon::Experiment::SetMeasurementTypeName}
#' @field createAntShapeType
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::CreateAntShapeType}{fort::myrmidon::Experiment::CreateAntShapeType}
#' @field antShapeTypeNames returns a \code{\link{AntShapeTypeByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::AntShapeTypeNames}{fort::myrmidon::Experiment::AntShapeTypeNames}
#' @field setAntShapeTypeName
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetAntShapeTypeName}{fort::myrmidon::Experiment::SetAntShapeTypeName}
#' @field deleteAntShapeType
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::DeleteAntShapeType}{fort::myrmidon::Experiment::DeleteAntShapeType}
#' @field addMetadataColumn
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::AddMetadataColumn}{fort::myrmidon::Experiment::AddMetadataColumn}
#' @field deleteMetadataColumn
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::DeleteMetadataColumn}{fort::myrmidon::Experiment::DeleteMetadataColumn}
#' @field antMetadataColumns
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::AntMetadataColumns}{fort::myrmidon::Experiment::AntMetadataColumns}
#' @field renameAntMetadataColumn
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::RenameAntMetadataColumn}{fort::myrmidon::Experiment::RenameAntMetadataColumn}
#' @field setAntMetadataColumnType
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::SetAntMetadataColumnType}{fort::myrmidon::Experiment::SetAntMetadataColumnType}
#' @field getDataInformations
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Experiment/fort::myrmidon::Experiment::GetDataInformations}{fort::myrmidon::Experiment::GetDataInformations}
NULL

#' @name fmCExperiment
#' @title a reference class wrapper for the C++
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment}{fort::myrmidon::CExperiment}
#'     class.
#' @field absoluteFilePath
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::AbsoluteFilePath}{fort::myrmidon::CExperiment::AbsoluteFilePath}
#' @field cSpaces returns a \code{\link{fmCSpaceByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::CSpaces}{fort::myrmidon::CExperiment::CSpaces}
#' @field cAnts returns a \code{\link{fmCAntByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::CAnts}{fort::myrmidon::CExperiment::CAnts}
#' @field freeIdentificationRangeAt
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::FreeIdentificationRangeAt}{fort::myrmidon::CExperiment::FreeIdentificationRangeAt}
#' @field name
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::Name}{fort::myrmidon::CExperiment::Name}
#' @field comment
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::Comment}{fort::myrmidon::CExperiment::Comment}
#' @field author
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::Author}{fort::myrmidon::CExperiment::Author}
#' @field family
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::Family}{fort::myrmidon::CExperiment::Family}
#' @field defaultTagSize
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::DefaultTagSize}{fort::myrmidon::CExperiment::DefaultTagSize}
#' @field threshold
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::Threshold}{fort::myrmidon::CExperiment::Threshold}
#' @field measurementTypeNames  returns a \code{\link{fmMeasurementTypeNameByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::MeasurementTypeNames}{fort::myrmidon::CExperiment::MeasurementTypeNames}
#' @field antShapeTypeNames returns a \code{\link{fmAntShapeTypeByID}} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::AntShapeTypeNames}{fort::myrmidon::CExperiment::AntShapeTypeNames}
#' @field antMetadataColumns
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::AntMetadataColumns}{fort::myrmidon::CExperiment::AntMetadataColumns}
#' @field getDataInformations
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CExperiment/fort::myrmidon::CExperiment::GetDataInformations}{fort::myrmidon::CExperiment::GetDataInformations}
NULL
