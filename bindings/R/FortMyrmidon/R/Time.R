#' Comparison operator for fmTime
#'
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} represents a time before \code{e2}
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:00:00.000Z") < fmTimeParse("2020-01-29T08:00:01.000Z")
#' # outputs TRUE
#' fmTimeParse("2020-01-29T08:00:00.000Z") < fmTimeParse("2020-01-29T08:00:00.000Z")
#' # outputs FALSE
#' }
`<.Rcpp_fmTime` <- function(e1, e2) (
    e1$before(e2)
)

#' Comparison operator for fmTime
#'
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} represents a time before or equal to \code{e2}
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:00:00.000Z") <= fmTimeParse("2020-01-29T08:00:01.000Z")
#' # outputs TRUE
#' fmTimeParse("2020-01-29T08:00:00.000Z") <= fmTimeParse("2020-01-29T08:00:00.000Z")
#' # outputs TRUE
#' }
`<=.Rcpp_fmTime` <- function(e1, e2) (
    !e1$after(e2)
)

#' Comparison operator for fmTime
#'
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} represents a time after \code{e2}
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:00:01.000Z") > fmTimeParse("2020-01-29T08:00:00.000Z")
#' # outputs TRUE
#' fmTimeParse("2020-01-29T08:00:00.000Z") > fmTimeParse("2020-01-29T08:00:00.000Z")
#' # outputs FALSE
#' }
`>.Rcpp_fmTime` <- function(e1, e2) (
    e1$after(e2)
)
#' Comparison operator for fmTime
#'
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} represents a time after or equal to \code{e2}
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:01:00.000Z") >= fmTimeParse("2020-01-29T08:00:00.000Z")
#' # outputs TRUE
#' fmTimeParse("2020-01-29T08:00:00.000Z") >= fmTimeParse("2020-01-29T08:00:00.000Z")
#' # outputs TRUE
#' }
`>=.Rcpp_fmTime` <- function(e1, e2) (
    !e1$before(e2)
)

#' Equal operator for fmTime
#'
#' @param e1 the first fmTime to compare
#' @param e2 the second fmTime to compare
#' @return \code{TRUE} if \code{e1} and  \code{e2} represents the same time at nanoseconds precision
#' @examples
#' \dontrun{
#' fmTimeParse("2020-01-29T08:00:00.000Z") == fmTimeParse("2020-01-29T08:00:00.000Z")
#' # outputs TRUE
#' fmTimeParse("2020-01-29T08:00:00.000Z") == fmTimeParse("2020-01-29T08:00:00.000Z")$add(1)
#' # outpus FALSE (1ns difference)
#' }
`==.Rcpp_fmTime` <- function(e1, e2) (
    e1$equals(e2)
)

`!=.Rcpp_fmTime` <- function(e1, e2) (
    !e1$equals(e2)
)


#' @name fmTime
#' @title a Reference Class that represents a point in Time
#' @description This is a wrapper for the C++ class \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time}{fort::myrmidon::Time}
#'
#' @field after
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::After}{fort::myrmidon::Time::After}
#' @field before
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Before}{fort::myrmidon::Time::Before}
#' @field equals
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Equals}{fort::myrmidon::Time::Equals}
#' @field add
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Add}{fort::myrmidon::Time::Add}
#' @field sub
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Sub}{fort::myrmidon::Time::Sub}
#' @field round
#'     \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Round}{fort::myrmidon::Time::Round}
#' @field const_ptr transform this fmTime to a
#'     \linkS4class{fmTimeCPtr}, an utility method for function that
#'     needs a \code{fort::myrmidon::Time::ConstPtr} as argument.
#' @field as.POSIXct convert this time to R object that represent a
#'     datetime.
NULL

#' @name fmTimeCPtr
#' @title a Reference Class that represents a pointer to a fmTime
#' @description in myrmiodn, many methods or static methods takes as
#'     argument a \code{fort::myrmidon::Time::ConstPtr} a pointer to a
#'     Time object. C++ \code{nullptr} for these methods represents
#'     +/-∞. R does not differentiate object and pointer, so this
#'     class is used to represent a pointer to an
#'     \linkS4class{fmTime}. use \code{\link{get}} method to get the
#'     underlying \linkS4class{fmTime} object.
#'
#' @field get return the underlying \linkS4class{fmTime} object.
NULL

#' @name fmTimeCPtrFromAnySEXP
#' @title Converts several object type to a \linkS4class{fmTimeCPtr}
#' @param exp an object that could be:
#'     \itemize{
#'     \item NULL to represent +/-∞
#'     \item a POSIXct/POSIXlt object from R
#'     \item a fmTimeCPtr, such as one returned by many myrmidon methods
#'     \item a fmTime such as outpute by \code{\link{fmTimeNow}} or \code{\link{fmTimeParse}}
#'     }
#' @return a \linkS4class{fmTimeCPtr} representing the wanted time.
NULL
