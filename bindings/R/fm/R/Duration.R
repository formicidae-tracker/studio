`+.Rcpp_fmDuration` <- function(e1,e2) (
    new (fm::fmDuration,e1$nanoseconds() + e2$nanoseconds() )
)

`-.Rcpp_fmDuration` <- function(e1,e2) (
    new(fm::fmDuration,e1$nanoseconds() - e2$nanoseconds())
)

`<.Rcpp_fmDuration` <- function(e1,e2) (
    e1$nanoseconds() < e2$nanoseconds()
)

`<=.Rcpp_fmDuration` <- function(e1,e2) (
    e1$nanoseconds() <= e2$nanoseconds()
)

`>=.Rcpp_fmDuration` <- function(e1,e2) (
    e1$nanoseconds() >= e2$nanoseconds()
)

`>.Rcpp_fmDuration` <- function(e1,e2) (
    e1$nanoseconds() > e2$nanoseconds()
)

`==.Rcpp_fmDuration` <- function(e1,e2) (
    e1$nanoseconds() == e2$nanoseconds()
)

`!=.Rcpp_fmDuration` <- function(e1,e2) (
    e1$nanoseconds() != e2$nanoseconds()
)


fmHours <- function (h) (
    new (fm::fmDuration, h * 3600 * 1e9 )
)

fmMinutes <- function (h) (
    new (fm::fmDuration, h * 60 * 1e9 )
)

fmSeconds <- function (h) (
    new (fm::fmDuration, h * 1e9 )
)

fmMilliseconds <- function (h) (
    new (fm::fmDuration, h * 1e6 )
)

fmMicroseconds <- function (h) (
    new (fm::fmDuration, h * 1e3 )
)

fmNanoseconds <- function (h) (
    new (fm::fmDuration, h )
)
