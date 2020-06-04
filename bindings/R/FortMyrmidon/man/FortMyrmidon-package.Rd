\name{FortMyrmidon-package}
\alias{FortMyrmidon-package}
\alias{FortMyrmidon}
\docType{package}
\title{
  A short title line describing what the package does
}
\description{
  A more detailed description of what the package does. A length
  of about one to five lines is recommended.
}
\details{
  This section should provide a more detailed overview of how to use the
  package, including the most important functions.
}
\author{
Your Name, email optional.

Maintainer: Your Name <your@email.com>
}
\references{
  This optional section can contain literature or other references for
  background information.
}
\keyword{ package }
\seealso{
  Optional links to other man pages
}
\examples{
  \dontrun{
    # opens an experiment in readonly mode
    e <- fmExperimentOpenReadOnly("somefile.myrmidon")
    # gets alls Ant position from the specified date until the end of the experiment
    positions <- fmQueryIdentifyFrame(e,
                                      start = fmTimeParse("2020-03-30T15:04:32.123Z"))

  }
}
