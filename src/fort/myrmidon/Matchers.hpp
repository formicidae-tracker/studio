#pragma once

#include <memory>

#include "Types.hpp"


namespace fort {
namespace myrmidon {
class Matcher;
}
}

/**
 * Formats a fort::myrmidon::Matcher.
 * @param out the stream to format to
 * @param m the fort::myrmidon::Matcher to format
 *
 * @return a reference to out
 */
std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Matcher & m);

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Matcher;
class Query;
}

/**
 * A Matcher reduces down the output of a Query with some criterion
 *
 * Base Matchers
 * =============
 *
 * One would use the following function to get a Matcher :
 *
 * * AntID() : one of the considered Ant in the result should
 *   match a given AntID
 * * AntMetaData() : one of the key-value user defined meta-data pair
 *                 should match.
 * * AntDistanceSmallerThan(),AntDistanceGreaterThan() : for
 *   interaction queries only, ensure some criterion for the distance
 *   between the two considedred Ant.
 * * AntAngleSmallerThan()/AntAngleGreaterThan() : for interaction
 *   queries only, ensure that angle between Ant meets some
 *   criterion.
 *
 * Combination
 * ===========
 *
 * Using And() or Or(), one can combine several Matcher together to
 * build more complex criterion. For example to build a Matcher that
 * matches ID `001` or `002`:
 *
 * c++ :
 * ```
 * using namespace fort::myrmidon;
 * auto m = Matcher::Or(Matcher::AntID(1),Matcher::AntID(2));
 * ```
 * python :
 * ```python
 * import py_fort_myrmidon as fm
 * m = fm.Matcher.Or(fm.Matcher.AntID(1),fm.Matcher.AntID(2))
 * ```
 * R :
 * ```R
 * library(FortMyrmidon)
 * m <- fmMatcherOr(fmMatcherAntID(1),fmMatcherAntID(2))
 * ```
 */
class Matcher {
public:
	/**
	 * A pointer to a Matcher
	 */
	typedef std::shared_ptr<Matcher> Ptr;
	/**
	 * Combines several Matcher together in conjunction
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.And(*args) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherAnd <- function(...) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @param matchers the matchers to combine
	 *
	 * @return a new Matcher which will match only when all matchers
	 *         also matches.
	 */
	static Ptr And(std::vector<Ptr> matchers);

	/**
	 * Combines several Matcher together in disjunction.
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.Or(*args) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherOr <- function(...) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @param matchers the matchers to combine
	 *
	 * @return a new Matcher which will match if any of the matchers
	 *         matches.
	 */
	static Ptr Or(std::vector<Ptr> matchers);

	/**
	 * Matches a given AntID
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.AntID(antID :int) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherAntID <- function(antID = 0) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @param antID the AntID to match against
	 *
	 * In case of interactions, matches any
	 * interaction with one of the Ant having antID.
	 *
	 * @return a Matcher that matches Ant with the given antID
	 */
	static Ptr AntID(AntID ID);

	/**
	 *  Matches a given user meta data key/value
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.AntMetaData(key :str, value :py_fort_myrmidon.AntStaticValue) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherAntMetaData <- function(key = '', value = false) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @param key the key to match against
	 * @param value the value to match against
	 *
	 * In case of interactions, matches any interaction with one of
	 * the Ant meeting the criterion.
	 *
	 * @return a Matcher that matches Ant with key is value.
	 */
	static Ptr AntMetaData(const std::string & key, const AntStaticValue & value);

	/**
	 * Matches a distance between two Ants
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.AntDistanceSmallerThan(distance: float) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherAntDistanceSmallerThan <- function(distance = 0.0) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @param distance the distance to be smaller.
	 *
	 * In case of Trajectory, it matches anything.
	 *
	 * @return a Matcher that matches when two Ant lies within the given
	 *         distance
	 */
	static Ptr AntDistanceSmallerThan(double distance);

	/**
	 * Matches a distance between two Ants
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.AntDistanceGreaterThan(distance: float) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherAntDistanceGreaterThan <- function(distance = 0.0) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @distance the distance to be greater.
	 *
	 * In case of Trajectories, it matches anything.
	 *
	 * @return a Matcher that matches two Ant further appart than
	 *         distance.
	 */
	static Ptr AntDistanceGreaterThan(double distance);

	/**
	 * Matches an absolute angle between two Ants
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.AntAngleSmallerThan(angle: float) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherAntAngleSmallerThan <- function(angle = 0.0) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @param angle the angle to be smaller (in radians).
	 *
	 * In case of trajectories, it matches anything.
	 *
	 * @return a Matcher that matches when the absolute angle between
	 *         two Ants is smaller than angle.
	 */
	static Ptr AntAngleSmallerThan(double angle);

	/**
	 * Matches an absolute angle between two Ants
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.AntAngleGreaterThan(angle: float) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherAntAngleGreaterThan <- function(angle = 0.0) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @param angle the angle to be greater to (in radians).
	 *
	 * In case of trajectories, it matches anything.
	 *
	 * @return a Matcher that matches when the absolute angle between
	 *         two Ants is greater than angle.
	 */
	static Ptr AntAngleGreaterThan(double angle);


	/**
	 * Matches an InteractionType
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.InteractionType(type1: int, type2: int) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherInteractionType <- function(type1 = 0, type2 = 0) # returns a Rcpp_fmMatcher
	 * ```
	 *
	 * @param type1 the first AntShapeTypeID to match
	 * @param type2 the second AntShapeTypeID to match
	 *
	 * Matches `(type1,type2)` and `(type2,type1)` interactions. In
	 * the case of trajectories, it matches anything.
	 *
	 * @return a Matcher that matches a given InteractionType or its
	 *         opposite.
	 */
	static Ptr InteractionType(AntShapeTypeID type1, AntShapeTypeID type2);



	/**
	 * Matches Ant displacement
	 *
	 * * python:
	 * ```python
	 * py_fort_myrmidon.Matcher.AntDisplacement(under :float, minimumGap: py_fort_myrmidon.Duration) -> py_fort_myrmidon.Matcher
	 * ```
	 * * R:
	 * ```R
	 * fmMatcherAntDisplacement <- function( under = 0.0, minimumGap = fmSeconds(0.0)) # returns a Rcpp_fmMatcher
	 * ```
	 * @param under maximal allowed displacement in pixels
	 * @param minimumGap minimal time gap
	 *
	 *
	 * Matches Trajectories and Interactions where Ant displacement
	 * between two consecutive position is smaller than under. If
	 * minimumGap is not zero, this check will be enforced only if
	 * there was at least minimumGap Time ellapsed between the two
	 * positions.
	 *
	 * @return a Matcher that reject large displacements in a tracking
	 *         gap.
	 */
	static Ptr AntDisplacement(double under , Duration minimumGap);


private:
	friend class Query;
	friend class fort::myrmidon::priv::Query;
	friend std::ostream & ::operator<<(std::ostream & out,
	                                   const fort::myrmidon::Matcher & m);

	// opaque pointer to implementation
	typedef std::shared_ptr<priv::Matcher> PPtr;


	// Private implementation constructor
	// @pMatcher opaque pointer to implementation
	//
	// User should not build a matcher directly, they must use this
	// class static methods instead.
	inline Matcher(const PPtr & pMatcher)
		: d_p(pMatcher) {
	}

	// Cast to opaque implementation
	//
	// @return an opaque <PPtr>
	PPtr ToPrivate() const;

	PPtr d_p;
};


} // namespace myrmidon
} // namespace fortoio
