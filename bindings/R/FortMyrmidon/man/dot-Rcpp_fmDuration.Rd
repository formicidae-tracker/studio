% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Duration.R
\name{-.Rcpp_fmDuration}
\alias{-.Rcpp_fmDuration}
\title{Substract operator for fmDuration}
\usage{
\method{-}{Rcpp_fmDuration}(e1, e2)
}
\arguments{
\item{e1}{the first fmDuration to substract}

\item{e2}{the second fmDuration to substract}
}
\value{
the difference of the two duration
}
\description{
Substract operator for fmDuration
}
\examples{
\dontrun{
fmSecond(1) - fmMillisecond(1)
# outputs 999ms
}
}
