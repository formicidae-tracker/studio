#' @name fmZoneByID
#' @title a \code{std::map} of \code{\link{fmZone}} by their ID
#' @description a \code{std::map} of \code{\link{fmZone}} by their
#'     ID. It looks like an R list, but the \code{[[} R operator will
#'     only report a value if the corresponding ID is in the list, and
#'     fails otherwise.
#' @field as.list returns the map as an actual list (to iterate over)
#' @field size returns the size of the list
#' @examples
#' \dontrun{
#' antByID[[42]] # gets the fmZone with ID 42. May fail if there are no fmZone with ID 42
#' antByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
#' }
NULL

#' @name fmCZoneByID
#' @title a \code{std::map} of \code{\link{fmCZone}} by their ID
#' @description a \code{std::map} of \code{\link{fmCZone}} by their
#'     ID. It looks like an R list, but the \code{[[} R operator will
#'     only report a value if the corresponding ID is in the list, and
#'     fails otherwise.
#' @field as.list returns the map as an actual list (to iterate over)
#' @field size returns the size of the list
#' @examples
#' \dontrun{
#' antByID[[42]] # gets the fmCZone with ID 42. May fail if there are no fmCZone with ID 42
#' antByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
#' }
NULL

#' @name fmSpace
#' @title a Reference Class wrapper for
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space}{fort::myrmidon::Space}
#'
#' @field spaceID
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space::SpaceID}{fort::myrmidon::Space::SpaceID}
#' @field name
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space::Name}{fort::myrmidon::Space::Name}
#' @field setName
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space::SetName}{fort::myrmidon::Space::SetName}
#' @field createZone
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space::CreateZone}{fort::myrmidon::Space::CreateZone}
#' @field deleteZone
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space::DeleteZone}{fort::myrmidon::Space::DeleteZone}
#' @field cZones returns a \linkS4class{fmCZoneByID} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space::CZones}{fort::myrmidon::Space::CZones}
#' @field zones returns a \linkS4class{fmZoneByID} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space::Zones}{fort::myrmidon::Space::Zones}
#' @field locateMovieFrame
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Space/fort::myrmidon::Space::LocateMovieFrame}{fort::myrmidon::Space::LocateMovieFrame}
NULL

#' @name fmCSpace
#' @title a Reference Class wrapper for
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CSpace/fort::myrmidon::CSpace}{fort::myrmidon::CSpace}
#'
#' @field spaceID
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CSpace/fort::myrmidon::CSpace::CSpaceID}{fort::myrmidon::CSpace::CSpaceID}
#' @field name
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CSpace/fort::myrmidon::CSpace::Name}{fort::myrmidon::CSpace::Name}
#' @field cZones returns a \linkS4class{fmCZoneByID} as
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CSpace/fort::myrmidon::CSpace::CZones}{fort::myrmidon::CSpace::CZones}
#' @field locateMovieFrame
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CSpace/fort::myrmidon::CSpace::LocateMovieFrame}{fort::myrmidon::CSpace::LocateMovieFrame}
NULL
