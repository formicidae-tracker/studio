% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Duration.R
\name{fmNanosecond}
\alias{fmNanosecond}
\title{Creates a fmDuration which is a multiple of a nanosecond}
\arguments{
\item{v}{the wanted number of nanoseconds}
}
\value{
a fmDuration representing the wanted amount of nanoseconds
}
\description{
Creates a fmDuration which is a multiple of a nanosecond
}
\examples{
fmNanosecond(2)
# outputs 2ns
fmNanosecond(1.1)
# outputs 1ns, nanoseconds is the smallest available value
}
