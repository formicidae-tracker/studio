\name{FortMyrmidon-package}
\alias{FortMyrmidon-package}
\alias{FortMyrmidon}
\docType{package}
\title{
  R wrapper for the C++ FORT Myrmdion API
}
\description{
  This package are R bindings to the C++ fort myrmidon API, an API to
  access and process the tracking data of the FORmicidae Tracker.
}
\details{
  The R documentation of this pacakge is rather quite
  non-complete. While it least all Classes and method accessible to R,
  most of it are pointers to the
  \href{https://formicidae-tracker.github.io/studio/docs/latest/api/}{FORT
	MYrmidon C++ API documentation}. Only some function (especially queries)
  have their own detailed R documentation. While reading the C++
  documentation, an R version could be found using the following rules:
  \itemize{
	\item any method, class name, function in R is prefixed with \code{fm}
	to replace the \code{fort::myrmidon::} C++ namespace. Example:
	\code{fort::myrmidon::Experiment}  becomes \code{fmExperiment}, or
	\code{fort::myrmidon::CAnt} becomes \code{fmCAnt}.
	\item any method from C++ uses the same name in R, at the exception of
	the first letter that do not use a capital, for example C++
	\code{fort::myrmidon::Ant::DisplayColor} becomes in R
	\code{fmAnt$displayColor}
	\item any static method of any class is exported by prefixing the
	class name in fromt of the function. Example
	\code{fort::myrmidon::Experiment::OpenReadOnly} becomes in R
	\code{fmExperimentOpenReadOnly}
	\item any enum in the C++ API have a corresponding list in R with
	the name correspondance. For example the
	\code{fort::myrmidon::AntMetadata::Type} have a corresponding
	\code{\link{fmAntMetadataType}} list.
  }
}
\author{
Alexandre Tuleu <alexandre.tuleu.2005@polytechnique.org>

Maintainer: Alexandre Tuleu. <alexandre.tuleu.2005@polytechnique.org>
}
\references{
  Documentation for the FORmicidae Tracker myrmidon API could be found
  at \url{https://formicidae-tracker.github.io/studio/}.

  You can join the
  \href{https://join.slack.com/t/formicidae-tracker/shared_invite/zt-ev4l54jm-~1g6yI_PMlYcv0bEv7M2VQ}{Slack
	workspace} to ask questions.

}
\keyword{ package }
\examples{
  \dontrun{
    # opens an experiment in readonly mode
    e <- fmExperimentOpenReadOnly("somefile.myrmidon")
    # gets alls Ant position from the specified date until the end of the experiment
    positions <- fmQueryIdentifyFrame(e,
                                      start = fmTimeParse("2020-03-30T15:04:32.123Z"))

  }
}
