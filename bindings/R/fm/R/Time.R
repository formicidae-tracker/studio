`<.Rcpp_fmTime` <- function(e1, e2) (
    e1$before(e2)
)

`<=.Rcpp_fmTime` <- function(e1, e2) (
    !e1$after(e2)
)

`>.Rcpp_fmTime` <- function(e1, e2) (
    e1$after(e2)
)

`>=.Rcpp_fmTime` <- function(e1, e2) (
    !e1$before(e2)
)

`==.Rcpp_fmTime` <- function(e1, e2) (
    e1$equals(e2)
)

`!=.Rcpp_fmTime` <- function(e1, e2) (
    !e1$equals(e2)
)
