% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Matchers.R
\name{fmMatcherAntDistanceSmallerThan}
\alias{fmMatcherAntDistanceSmallerThan}
\title{\code{\link{fmMatcher}} a matcher that matches when a
    distance between two ant are gretaer than a threshold}
\arguments{
\item{threshold}{the distance threshold}
}
\value{
a \code{\link{fmMatcher}} a matcher that matches when the
    distance between two ant is smaller than the threshold. Only
    valid for query on collision or interaction, and always matches
    for position or trajectory query.
}
\description{
\code{\link{fmMatcher}} a matcher that matches when a
    distance between two ant are gretaer than a threshold
}
\examples{
# a matches when two ant are more than 30 pixels appart
fmMatcherAntDistanceSmallerThan(30)
}
