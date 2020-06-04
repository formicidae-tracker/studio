#' @name fmCAnt
#' @title A reference class that represent a non-modifiable Ant
#' @description This is a wrapper for the C++ class
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CAnt}{fort::myrmidon::CAnt}
#'
#' @field cIdentifications
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CAnt/fort::myrmidon::CAnt::CIdentifications}{fort::myrmidon::CAnt::CIdentifications}
#' @field antID
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CAnt/fort::myrmidon::CAnt::AntID}{fort::myrmidon::CAnt::AntID}
#' @field formattedID
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CAnt/fort::myrmidon::CAnt::FormattedID}{fort::myrmidon::CAnt::FormattedID}
#' @field displayColor
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CAnt/fort::myrmidon::CAnt::DisplayColor}{fort::myrmidon::CAnt::DisplayColor}
#' @field displayStatus
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CAnt/fort::myrmidon::CAnt::DisplayStatus}{fort::myrmidon::CAnt::DisplayStatus}
#' @field getValue
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CAnt/fort::myrmidon::CAnt::GetValue}{fort::myrmidon::CAnt::GetValue}
#' @field capsules wrapper for
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::CAnt/fort::myrmidon::CAnt::Capsules}{fort::myrmidon::CAnt::Capsules}. It
#'     returns a list with two elements, the 'capsules' element, a
#'     list of \linkS4class{fmCapsule} and 'shapeTypes' their
#'     corresponding shape types.
NULL


#' @name fmAnt
#' @title A reference class that represent an Ant
#' @description This is a wrapper for the C++ class
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant}{fort::myrmidon::Ant}
#'
#' @field cIdentifications
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::CIdentifications}{fort::myrmidon::Ant::CIdentifications}
#' @field identifications
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::Identifications}{fort::myrmidon::Ant::Identifications}
#' @field antID
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::AntID}{fort::myrmidon::Ant::AntID}
#' @field formattedID
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::FormattedID}{fort::myrmidon::Ant::FormattedID}
#' @field displayColor
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::DisplayColor}{fort::myrmidon::Ant::DisplayColor}
#' @field setDisplayColor
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::SetDisplayColor}{fort::myrmidon::Ant::SetDisplayColor}
#' @field displayStatus
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::DisplayStatus}{fort::myrmidon::Ant::DisplayStatus}
#' @field getValue
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::GetValue}{fort::myrmidon::Ant::GetValue}
#' @field deleteValue
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::DeleteValue}{fort::myrmidon::Ant::DeleteValue}
#' @field setValue
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::SetValue}{fort::myrmidon::Ant::SetValue}
#' @field addCapsule
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::AddCapsule}{fort::myrmidon::Ant::AddCapsule}
#' @field capsules wrapper for
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::Capsules}{fort::myrmidon::Ant::Capsules}. It
#'     returns a list with two elements, the 'capsules' element, a
#'     list of \linkS4class{fmCapsule} and 'shapeTypes' their
#'     corresponding shape types.
#' @field deleteCapsule
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::DeleteCapsule}{fort::myrmidon::Ant::DeleteCapsule}
#' @field clearCapsules
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::ClearCapsules}{fort::myrmidon::Ant::ClearCapsules}
#' @field setDisplayStatus
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Ant/fort::myrmidon::Ant::SetDisplayStatus}{fort::myrmidon::Ant::SetDisplayStatus}
NULL
