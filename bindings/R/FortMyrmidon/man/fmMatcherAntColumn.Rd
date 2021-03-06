% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Matchers.R
\name{fmMatcherAntColumn}
\alias{fmMatcherAntColumn}
\title{\code{\link{fmMatcher}} a matcher that matches when an
    fmAntStaticValue meets the desired value}
\arguments{
\item{name}{the column name to match}

\item{value}{the column name to match}
}
\value{
a \code{\link{fmMatcher}} a matcher that matches when the
    value of an ant matches the criterion
}
\description{
\code{\link{fmMatcher}} a matcher that matches when an
    fmAntStaticValue meets the desired value
}
\examples{
# a fmMatcher that matches when ant value "group" is "nurses"
fmMatcherAntColumn("group",fmAntStaticString("nurses"))
}
