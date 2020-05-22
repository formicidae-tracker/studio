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
