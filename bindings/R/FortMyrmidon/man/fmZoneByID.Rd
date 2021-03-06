% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Space.R
\name{fmZoneByID}
\alias{fmZoneByID}
\title{a \code{std::map} of \code{\link{fmZone}} by their ID}
\description{
a \code{std::map} of \code{\link{fmZone}} by their
    ID. It looks like an R list, but the \code{[[} R operator will
    only report a value if the corresponding ID is in the list, and
    fails otherwise.
}
\section{Fields}{

\describe{
\item{\code{as.list}}{returns the map as an actual list (to iterate over)}

\item{\code{size}}{returns the size of the list}
}}

\examples{
\dontrun{
antByID[[42]] # gets the fmZone with ID 42. May fail if there are no fmZone with ID 42
antByID.as.list # returns the actual R list, but we won't be able to query for a specific ID.
}
}
