#' @name fmZoneDefinition
#' @title Reference Class wrapper for the C++
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::ZoneDefinition/fort::myrmidon::ZoneDefinition}{fort::myrmidon::ZoneDefinition}
#'     class
#'
#' @field geometry
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::ZoneDefinition/fort::myrmidon::ZoneDefinition::Geometry}{fort::myrmidon::ZoneDefinition::Geometry}
#' @field setGeometry
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::ZoneDefinition/fort::myrmidon::ZoneDefinition::SetGeometry}{fort::myrmidon::ZoneDefinition::SetGeometry}
#' @field start
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::ZoneDefinition/fort::myrmidon::ZoneDefinition::Start}{fort::myrmidon::ZoneDefinition::Start}
#' @field end
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::ZoneDefinition/fort::myrmidon::ZoneDefinition::End}{fort::myrmidon::ZoneDefinition::End}
#' @field setStart
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::ZoneDefinition/fort::myrmidon::ZoneDefinition::SetStart}{fort::myrmidon::ZoneDefinition::SetStart}
#' @field setEnd
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::ZoneDefinition/fort::myrmidon::ZoneDefinition::SetEnd}{fort::myrmidon::ZoneDefinition::SetEnd}
#' @examples
#' \dontrun{
#' # sets the geomtery of a zone zd to two circle
#' zd$setGeometry(list(fmCircleCreate(c(0,0),3),fmCircleCreate(c(10,10,10))))
#' }
NULL

#' @name fmCZoneDefinition
#' @title Reference Class wrapper for the C++
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::XCZoneDefinition/fort::myrmidon::CZoneDefinition}{fort::myrmidon::CZoneDefinition}
#'     class
#'
#' @field geometry
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CZoneDefinition/fort::myrmidon::CZoneDefinition::Geometry}{fort::myrmidon::CZoneDefinition::Geometry}
#' @field start
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CZoneDefinition/fort::myrmidon::CZoneDefinition::Start}{fort::myrmidon::CZoneDefinition::Start}
#' @field end
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CZoneDefinition/fort::myrmidon::CZoneDefinition::End}{fort::myrmidon::CZoneDefinition::End}
NULL

#' @name fmZone
#' @title Reference Class wrapper for the c++
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Zone/fort::myrmidon::Zone}{fort::myrmidon::Zone}
#'     class
#'
#' @field addDefinition
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Zone/fort::myrmidon::Zone::AddDefinition}{fort::myrmidon::Zone::AddDefinition}
#' @field cDefinitions
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Zone/fort::myrmidon::Zone::CDefinitions}{fort::myrmidon::Zone::CDefinitions}
#' @field definitions
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Zone/fort::myrmidon::Zone::Definitions}{fort::myrmidon::Zone::Definitions}
#' @field eraseDefinition
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Zone/fort::myrmidon::Zone::EraseDefinition}{fort::myrmidon::Zone::EraseDefinition}
#' @field name
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Zone/fort::myrmidon::Zone::Name}{fort::myrmidon::Zone::Name}
#' @field setName
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Zone/fort::myrmidon::Zone::SetName}{fort::myrmidon::Zone::SetName}
#' @field zoneID
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Zone/fort::myrmidon::Zone::ZoneID}{fort::myrmidon::Zone::ZoneID}
NULL


#' @name fmCZone
#' @title Reference Class wrapper for the c++
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CZone/fort::myrmidon::CZone}{fort::myrmidon::CZone}
#'     class
#'
#' @field cDefinitions
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CZone/fort::myrmidon::CZone::CDefinitions}{fort::myrmidon::CZone::CDefinitions}
#' @field name
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CZone/fort::myrmidon::CZone::Name}{fort::myrmidon::CZone::Name}
#' @field zoneID
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CZone/fort::myrmidon::CZone::ZoneID}{fort::myrmidon::CZone::ZoneID}
NULL
