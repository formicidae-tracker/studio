#' @name fmShape
#' @title a Refrence class for any kind of shape
#' @description just defined for C++ jibber-jabber
NULL

#' @name fmCircle
#' @title a Reference Class that wraps a C++ \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Circle/fort::myrmidon::Circle}{fort::myrmidon::Circle}
#'
#' @field center
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Circle/fort::myrmidon::Circle::Center}{fort::myrmidon::Circle::Center}
#' @field radius
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Circle/fort::myrmidon::Circle::Radius}{fort::myrmidon::Circle::Radius}
#' @field setCenter
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Circle/fort::myrmidon::Circle::SetCenter}{fort::myrmidon::Circle::SetRadius}
#' @field setRadius
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Circle/fort::myrmidon::Circle::SetRadius}{fort::myrmidon::Circle::SetRadius}
#'
#' @examples
#' # creates a fmCircle of center 1,1 amnd radius 2
#' c <- fmCircleCreate(c(1,1),2)
#' # moves it to (0,0)
#' c$setCenter(c(0,0))
#'
NULL

#' @name fmCapsule
#' @title a Reference Class that wraps a C++ \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule}{fort::myrmidon::Capsule}
#'
#' @field c1
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule::C1}{fort::myrmidon::Capsule::C1}
#' @field c2
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule::C2}{fort::myrmidon::Capsule::C2}
#' @field r1
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule::R1}{fort::myrmidon::Capsule::R1}
#' @field r2
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule::R2}{fort::myrmidon::Capsule::R2}
#' @field setC1
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule::SetC1}{fort::myrmidon::Capsule::SetC1}
#' @field setC2
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule::SetC2}{fort::myrmidon::Capsule::SetC2}
#' @field setR1
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule::SetR1}{fort::myrmidon::Capsule::SetR1}
#' @field setR2
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Capsule/fort::myrmidon::Capsule::SetR2}{fort::myrmidon::Capsule::SetR2}
#' @examples
#' # creates a fmCapsule of center (1,1) and (2,2) radius 0.5 and 0.4
#' c <- fmCapsuleCreate(c(1,1),c(2,2),0.5,0.4)
#' # moves second center to (0,0)
#' c$setC2(c(0,0))
NULL

#' @name fmPolygon
#' @title a Reference Class that wraps a C++ \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Polygon/fort::myrmidon::Capsule}{fort::myrmidon::Polygon}
#'
#' @field size
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Polygon/fort::myrmidon::Polygon::Size}{fort::myrmidon::Polygon::Size}
#' @field vertex
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Polygon/fort::myrmidon::Polygon::Vertex}{fort::myrmidon::Polygon::Vertex}
#' @field setVertex
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Polygon/fort::myrmidon::Polygon::SetVertex}{fort::myrmidon::Polygon::SetVertex}
#' @examples
#' #creates a nice (0,0) centered square of size 2
#' p <- fmPolygonCreate(list(c(-1,-1),c(1,-1),c(1,1),c(-1,1)))
#' # transforms it into the extinction rebellion symbol by swapping the
#' # two lastelements, as order matters here.
#' # ATTENTION  list index starts at 0, like in C
#' p$setVertex(2,c(-1,1))
#' p$setVertex(3,c(1,1))
NULL
