#' @name fmMatcher
#' @title Reference abstract class to match criterion on queries
#' @description fmMatchers can be used to match against certain
#'     criterion, here are the available matcher:
#' \itemize{
#' \item \code{\link{fmMatcherAny}}
#' \item \code{\link{fmMatcherAnd}}
#' \item \code{\link{fmMatcherOr}}
#' \item \code{\link{fmMatcherAntID}}
#' \item \code{\link{fmMatcherAntColumn}}
#' \item \code{\link{fmMatcherAntDistanceGreaterThan}}
#' \item \code{\link{fmMatcherAntDistanceSmallerThan}}
#' \item \code{\link{fmMatcherAntAngleSmallerThan}}
#' \item \code{\link{fmMatcherAntAngleGreaterThan}}
#' }
#'
NULL

#' @name fmMatcherAny
#' @title \code{\link{fmMatcher}} a matcher tha matches anything
#' @return a \code{\link{fmMatcher}} a matcher that matches anything
NULL

#' @name fmMatcherAnd
#' @title \code{\link{fmMatcher}} a matcher that matches all other
#'     matchers
#' @param matchers a list of other \code{\link{fmMatcher}} to match
#' @return a \code{\link{fmMatcher}} a matcher that matches when all
#'     subMatcher matches
#' @examples
#' # a fmMatcher that matches two criterion (could be more)
#' fmMatcherAnd(list(fmMatcherAntColumn("group",fmAntStaticString("worker")),fmMatcherAntID(42)))
NULL

#' @name fmMatcherOr
#' @title \code{\link{fmMatcher}} a matcher that matches any other
#'     matchers
#' @param matchers a list of other \code{\link{fmMatcher}} to match
#' @return a \code{\link{fmMatcher}} a matcher that matches when any
#'     sub-fmMatcher matches
#' @examples
#' # a fmMatcher that matches any two criterion (could be more)
#' fmMatcherOr(list(fmMatcherAntColumn("group",
#'                                     fmAntStaticString("workers")),
#'                  fmMatcherAntColumn("group",
#'                                     fmAntStaticString("nurses"))))
NULL

#' @name fmMatcherAntID
#' @title \code{\link{fmMatcher}} a matcher that matches ant with
#'     given ID
#' @param ID the ID to match
#' @return a \code{\link{fmMatcher}} a matcher that matches when any
#'     ant has the given ID
#' @examples
#' # a fmMatcher that matches ant ID 42
#' fmMatcherAntID(42)
NULL

#' @name fmMatcherAntColumn
#' @title \code{\link{fmMatcher}} a matcher that matches when an
#'     fmAntStaticValue meets the desired value
#' @param name the column name to match
#' @param value the column name to match
#' @return a \code{\link{fmMatcher}} a matcher that matches when the
#'     value of an ant matches the criterion
#' @examples
#' # a fmMatcher that matches when ant value "group" is "nurses"
#' fmMatcherAntColumn("group",fmAntStaticString("nurses"))
NULL

#' @name fmMatcherAntDistanceGreaterThan
#' @title \code{\link{fmMatcher}} a matcher that matches when a
#'     distance between two ant are gretaer than a threshold
#' @param threshold the distance threshold
#' @return a \code{\link{fmMatcher}} a matcher that matches when the
#'     distance between two ant is greater than the threshold. Only
#'     valid for query on collision or interaction, and always matches
#'     for position or trajectory query.
#' @examples
#' # a matches when two ant are more than 30 pixels appart
#' fmMatcherAntDistanceGreaterThan(30)
NULL

#' @name fmMatcherAntDistanceSmallerThan
#' @title \code{\link{fmMatcher}} a matcher that matches when a
#'     distance between two ant are gretaer than a threshold
#' @param threshold the distance threshold
#' @return a \code{\link{fmMatcher}} a matcher that matches when the
#'     distance between two ant is smaller than the threshold. Only
#'     valid for query on collision or interaction, and always matches
#'     for position or trajectory query.
#' @examples
#' # a matches when two ant are more than 30 pixels appart
#' fmMatcherAntDistanceSmallerThan(30)
NULL

#' @name fmMatcherAntAngleGreaterThan
#' @title \code{\link{fmMatcher}} a matcher that matches when a
#'     angle between two ant are gretaer than a threshold
#' @param threshold the angle threshold
#' @return a \code{\link{fmMatcher}} a matcher that matches when the
#'     angle between two ant is greater than the threshold. Only
#'     valid for query on collision or interaction, and always matches
#'     for position or trajectory query.
#' @examples
#' # a matches when two ant are more than 30 pixels appart
#' fmMatcherAntAngleGreaterThan(30)
NULL

#' @name fmMatcherAntAngleSmallerThan
#' @title \code{\link{fmMatcher}} a matcher that matches when a
#'     angle between two ant are gretaer than a threshold
#' @param threshold the angle threshold
#' @return a \code{\link{fmMatcher}} a matcher that matches when the
#'     angle between two ant is smaller than the threshold. Only
#'     valid for query on collision or interaction, and always matches
#'     for position or trajectory query.
#' @examples
#' # a matches when two ant are more than 30 pixels appart
#' fmMatcherAntAngleSmallerThan(30)
NULL

#' @name fmMatcherInteractionType
#' @title \code{\link{fmMatcher}} a matcher that matches when an
#'     interaction contains the given type
#' @param type1 the first virtual ant shape type to match
#' @param type2 the second virtual ant shape type to match
#' @return a \code{\link{fmMatcher}} a matcher that matches when the
#'     interaction contains the given type. Matches (type1,type2) and
#'     (type2,type1). Only valid for query on collision or
#'     interaction, and always matches for position or trajectory
#'     queries.
#' @examples
#' # matches 1 <-> 2 and 2 <-> 1 interactions
#' fmMatcherInteractionType(1,2)
NULL
