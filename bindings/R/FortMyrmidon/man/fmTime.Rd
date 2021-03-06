% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Time.R
\name{fmTime}
\alias{fmTime}
\title{a Reference Class that represents a point in Time}
\description{
This is a wrapper for the C++ class \href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time}{fort::myrmidon::Time}
}
\section{Fields}{

\describe{
\item{\code{after}}{\href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::After}{fort::myrmidon::Time::After}}

\item{\code{before}}{\href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Before}{fort::myrmidon::Time::Before}}

\item{\code{equals}}{\href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Equals}{fort::myrmidon::Time::Equals}}

\item{\code{add}}{\href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Add}{fort::myrmidon::Time::Add}}

\item{\code{sub}}{\href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Sub}{fort::myrmidon::Time::Sub}}

\item{\code{round}}{\href{https://formicidae-tracker.github.io/studio/docs/latest/api/#fort::myrmidon::Time/fort::myrmidon::Time::Round}{fort::myrmidon::Time::Round}}

\item{\code{const_ptr}}{transform this fmTime to a
\linkS4class{fmTimeCPtr}, an utility method for function that
needs a \code{fort::myrmidon::Time::ConstPtr} as argument.}

\item{\code{as.POSIXct}}{convert this time to R object that represent a
datetime.}
}}

