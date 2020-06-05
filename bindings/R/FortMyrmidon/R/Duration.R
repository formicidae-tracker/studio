#' Addition operator for fmDuration
#'
#' @param e1 the first fmDuration to add
#' @param e2 the second fmDuration to add
#' @return the sum of the two fmDuration
#' @examples
#' \dontrun{
#' fmSecond(2) + fmMicrosecond(300)
#' # outputs 2.0003s
#' }
`+.Rcpp_fmDuration` <- function(e1, e2) (
    fmNanosecond(e1$nanoseconds() + e2$nanoseconds())
)

#' Substract operator for fmDuration
#'
#' @param e1 the first fmDuration to substract
#' @param e2 the second fmDuration to substract
#' @return the difference of the two duration
#' @examples
#' \dontrun{
#' fmSecond(1) - fmMillisecond(1)
#' # outputs 999ms
#' }
`-.Rcpp_fmDuration` <- function(e1, e2) (
    fmNanosecond(e1$nanoseconds() - e2$nanoseconds())
)

#' Comparison operator for fmDuration
#'
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 < e2}
#' @examples
#' \dontrun{
#' fmSecond(1) < fmMinute(1)
#' # outputs TRUE
#' fmSecond(60) < fmMinute(1)
#' # outputs FALSE (as 60s == 1m)
#' }
`<.Rcpp_fmDuration` <- function(e1, e2) (
    e1$nanoseconds() < e2$nanoseconds()
)

#' Comparison operator for fmDuration
#'
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 <= e2}
#' @examples
#' \dontrun{
#' fmSecond(1) <= fmMinute(1)
#' # outputs TRUE
#' fmSecond(60) <= fmMinute(1)
#' # outputs TRUE (as 60s == 1m)
#' }
`<=.Rcpp_fmDuration` <- function(e1, e2) (
    e1$nanoseconds() <= e2$nanoseconds()
)

#' Comparison operator for fmDuration
#'
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 >= e2}
#' @examples
#' \dontrun{
#' fmMinute(1) >= fmSecond(1)
#' # outputs TRUE
#' fmMinute(60) >= fmSecond(60)
#' # outputs TRUE (as 60s == 1m)
#' }
`>=.Rcpp_fmDuration` <- function(e1, e2) (
    e1$nanoseconds() >= e2$nanoseconds()
)

#' Comparison operator for fmDuration
#'
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 > e2}
#' @examples
#' \dontrun{
#' fmMinute(1) > fmSecond(1)
#' # outputs TRUE
#' fmMinute(1) > fmSecond(60)
#' # outputs FALSE (as 60s == 1m)
#' }
`>.Rcpp_fmDuration` <- function(e1, e2) (
    e1$nanoseconds() > e2$nanoseconds()
)

#' Equal operator for fmDuration
#'
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 == e2}
#' @examples
#' \dontrun{
#' fmSecond(1) == fmMinute(1)
#' # outputs FALSE
#' fmSecond(60) == fmMinute(1)
#' # outputs TRUE (as 60s == 1m)
#' }
`==.Rcpp_fmDuration` <- function(e1, e2) (
    e1$nanoseconds() == e2$nanoseconds()
)

#' Different operator for fmDuration
#'
#' @param e1 the first fmDuration to compare
#' @param e2 the second fmDuration to compare
#' @return \code{TRUE} if \code{e1 != e2}
#' @examples
#' \dontrun{
#' fmSecond(1) != fmMinute(1)
#' # outputs TRUE
#' fmSecond(60) != fmMinute(1)
#' # outputs FALSE (as 60s == 1m)
#' }
`!=.Rcpp_fmDuration` <- function(e1, e2) (
    e1$nanoseconds() != e2$nanoseconds()
)

#' @name fmHour
#' @title Creates a fmDuration which is a multiple of an hour
#'
#' @param v the wanted number of hours
#' @return a fmDuration representing the wanted amount of hours
#' @examples
#' fmHour(2)
#' # outputs 2h
#' fmHour(1.5)
#' # outputs 1h30m
NULL

#' @name fmMinute
#' @title Creates a fmDuration which is a multiple of a minute
#'
#' @param v the wanted number of minutes
#' @return a fmDuration representing the wanted amount of minutes
#' @examples
#' fmMinute(2)
#' # outputs 2m
#' fmMinute(1.5)
#' # outputs 1m30s
NULL

#' @name fmSecond
#' @title Creates a fmDuration which is a multiple of a second
#'
#' @param v the wanted number of second
#' @return a fmDuration representing the wanted amount of seconds
#' @examples
#' fmSecond(2)
#' # outputs 2s
#' fmSecond(1.5)
#' # outputs 1.5s
NULL

#' @name fmMillisecond
#' @title Creates a fmDuration which is a multiple of a millisecond
#'
#' @param v the wanted number of milliseconds
#' @return a fmDuration representing the wanted amount of milliseconds
#' @examples
#' fmMillisecond(2)
#' # outputs 2ms
#' fmMillisecond(1.5)
#' # outputs 1.5ms
NULL

#' @name fmMicrosecond
#' @title Creates a fmDuration which is a multiple of a microseconds
#'
#' @param v the wanted number of microseconds
#' @return a fmDuration representing the wanted amount of microseconds
#' @examples
#' fmMicrosecond(2)
#' # outputs 2us
#' fmMinute(1.5)
#' # outputs 1.5us
NULL

#' @name fmNanosecond
#' @title Creates a fmDuration which is a multiple of a nanosecond
#'
#' @param v the wanted number of nanoseconds
#' @return a fmDuration representing the wanted amount of nanoseconds
#' @examples
#' fmNanosecond(2)
#' # outputs 2ns
#' fmNanosecond(1.1)
#' # outputs 1ns, nanoseconds is the smallest available value
NULL

#' @name fmDuration
#' @title A Reference Class that represent a duration
#'
#' @description This a wrapper for \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Duration/fort::myrmidon::Duration}{fort::myrmidon::Duration}
#'
#' @field hours \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Duration/fort::myrmidon::Duration::Hours}{fort::myrmidon::Duration::Hours}
#' @field minutes \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Duration/fort::myrmidon::Duration::Minutes}{fort::myrmidon::Duration::Minutes}
#' @field seconds \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Duration/fort::myrmidon::Duration::Seconds}{fort::myrmidon::Duration::Seconds}
#' @field milliseconds \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Duration/fort::myrmidon::Duration::Milliseconds}{fort::myrmidon::Duration::Milliseconds}
#' @field microseconds \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Duration/fort::myrmidon::Duration::Microseconds}{fort::myrmidon::Duration::Microseconds}
#' @field Nanoseconds \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Duration/fort::myrmidon::Duration::Nanoseconds}{fort::myrmidon::Duration::Nanoseconds}
#'
NULL
