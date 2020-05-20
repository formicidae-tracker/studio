#pragma once

#include <memory>

#include "Types.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Matcher;
}

// Reduces down output of Queries
//
// Matchers reduces down output of <Qeury> with some criterions.
//
// * <AntIDMatcher> : <AntID> must match a given ID
// * <AntColumnMatcher> : an <AntStaticValue> must match a given value.
// * <AntDistanceSmallerThan>,<AntDistanceGreaterTham> : for
//   interaction queries anly, ensure some criterion for the distance
//   between <Ant>.
// * <AntAngleSmallerThan>/<AntAngleGreaterThan> : for interaction
//   queries only, ensure that angle between <Ant> meets sopme
//   criterion.
//
// ## Combination
//
// Using <And> or <Or>, one can combine several Matcher together to
// build more complex criterion.
class Matcher {
public:
	// A pointer to a Matcher
	typedef std::shared_ptr<Matcher> Ptr;

	// AND combines several Matcher together.
	// @matchers the matchers to combine
	//
	// AND combines several Matcher together. The resulting Matcher
	// will match only if all of the given Matcher matches too.
	// @return a new Matcher which is the AND combination of the given
	//         Matchers.
	static Ptr And(std::initializer_list<Ptr> matchers);

	// OR combines several Matcher together.
	// @matchers the matchers to combine
	//
	// OR combines several Matcher together. The resulting Matcher
	// will match if any of the given Matcher matches too.
	// @return a new Matcher which is the OR combination of the given
	//         Matchers.
	static Ptr Or(std::initializer_list<Ptr> matchers);

	// Matches a given <AntID>
	// @ID the <AntID> to matches against
	//
	// Matches a given <AntID>. In case of interactions, matches any
	// interaction with one of the Ant with the given <ID>
	//
	// @return a <Ptr> that matches <Ant>  of the given ID
	static Ptr AntIDMatcher(AntID ID);

	// Matches a given <AntStaticValue>
	// @name the name of the column to match against
	// @value the value to match against
	//
	// Matches a given <AntStaticValue>. In case of interactions,
	// matches any interaction with one of the Ant meeting the
	// criterion.
	//
	// @return a <Ptr> that matches <Ant> with the value matching that
	// <Ant>
	static Ptr AntColumnMatcher(const std::string & name, const AntStaticValue & value);

	// Matches a distance between two Ants
	// @distance the distance to be smaller.
	//
	// Matches that a distance between two Ants is smaller than
	// <distance>, only valid for interaction queries.
	//
	// @return a <Ptr> that matches two <Ant> within the given
	// <distance>
	static Ptr AntDistanceSmallerThan(double distance);

	// Matches a distance between two Ants
	// @distance the distance to be greater.
	//
	// Matches that a distance between two Ants is greater than
	// <distance>, only valid for interaction queries.
	//
	// @return a <Ptr> that matches two <Ant> outside the given
	// <distance>
	static Ptr AntDistanceGreaterThan(double distance);

	// Matches an angle between two Ants
	// @angle the angle to be smaller (in radians).
	//
	// Matches that the angle between two Ants is smaller than
	// <angle>, only valid for interaction queries.
	//
	// @return a <Ptr> that matches when two <Ant> are facing the same
	// direction with <angle> threshold.
	static Ptr AntAngleSmallerThan(double angle);

	// Matches an angle between two Ants
	// @angle the angle to be greater (in radians).
	//
	// Matches that the angle between two Ants is greater than
	// <angle>, only valid for interaction queries.
	//
	// @return a <Ptr> that matches when two <Ant> are not facing the
	// same direction with <angle> threshold.
	static Ptr AntAngleGreaterThan(double angle);

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
	// Casts down to private implementation
	//
	// @return the opaque pointer implementation.
	inline const PPtr & ToPrivate() const {
		return d_p;
	}
private:
	PPtr d_p;
};


} // namespace myrmidon
} // namespace fortoio
