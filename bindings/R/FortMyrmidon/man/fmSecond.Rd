% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Duration.R
\name{fmSecond}
\alias{fmSecond}
\title{Creates a fmDuration which is a multiple of a second}
\arguments{
\item{v}{the wanted number of second}
}
\value{
a fmDuration representing the wanted amount of seconds
}
\description{
Creates a fmDuration which is a multiple of a second
}
\examples{
fmSecond(2)
# outputs 2s
fmSecond(1.5)
# outputs 1.5s
}
