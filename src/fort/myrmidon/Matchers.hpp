#pragma once

#include <memory>

#include "Types.hpp"


namespace fort {
namespace myrmidon {
class Matcher;
}
}

// Formats a <fort::myrmidon::Matcher>.
// @out the stream to format to
// @m the <fort::myrmidon::Matcher> to format
//
// @return a reference to <out>
std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Matcher & m);

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Matcher;
}

// Reduces down output of Queries
//
// Matchers reduces down output of <Query> with some criterions.
//
// * <AntID> : <Ant::ID> must match a given ID
// * <AntColumn> : an <AntStaticValue> must match a given value.
// * <AntDistanceSmallerThan>,<AntDistanceGreaterThan> : for
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
	//
	// R Version :
	// ```R
	// fmMatcherAnd(list(...))
	// ```
	//
	// @return a new Matcher which is the AND combination of the given
	//         Matchers.
	static Ptr And(std::vector<Ptr> matchers);

	// OR combines several Matcher together.
	// @matchers the matchers to combine
	//
	// OR combines several Matcher together. The resulting Matcher
	// will match if any of the given Matcher matches too.
	//
	// R Version :
	// ```R
	// fmMatcherOr(list(...))
	// ```
	//
	// @return a new Matcher which is the OR combination of the given
	//         Matchers.
	static Ptr Or(std::vector<Ptr> matchers);

	// Matches a given <AntID>
	// @ID the <AntID> to matches against
	//
	// Matches a given <AntID>. In case of interactions, matches any
	// interaction with one of the Ant with the given <ID>
	//
	// R Version :
	// ```R
	// fmMatcherAntID(andID)
	// ```
	//
	// @return a Matcher that matches <Ant> with the given ID
	static Ptr AntID(AntID ID);

	// Matches a given <AntStaticValue>
	// @name the name of the column to match against
	// @value the value to match against
	//
	// Matches a given <AntStaticValue>. In case of interactions,
	// matches any interaction with one of the Ant meeting the
	// criterion.
	//
	// R Version :
	// ```R
	// fmMatcherAntColumn("group",fmAntStaticString("nurses"))
	// ```
	//
	// @return a Matcher that matches <Ant> with the value matching that
	// <Ant>
	static Ptr AntColumn(const std::string & name, const AntStaticValue & value);

	// Matches a distance between two Ants
	// @distance the distance to be smaller.
	//
	// Matches that a distance between two Ants is smaller than
	// <distance>, only valid for interaction queries.
	//
	// R Version :
	// ```R
	// fmMatcherAntDistanceSmallerThan(distance)
	// ```
	//
	// @return a Matcher that matches two <Ant> within the given
	// <distance>
	static Ptr AntDistanceSmallerThan(double distance);

	// Matches a distance between two Ants
	// @distance the distance to be greater.
	//
	// Matches that a distance between two Ants is greater than
	// <distance>, only valid for interaction queries.
	//
	// R Version :
	// ```R
	// fmMatcherAntDistanceGreaterThan(distance)
	// ```
	//
	// @return a Matcher that matches two <Ant> outside the given
	// <distance>
	static Ptr AntDistanceGreaterThan(double distance);

	// Matches an angle between two Ants
	// @angle the angle to be smaller (in radians).
	//
	// Matches that the angle between two Ants is smaller than
	// <angle>, only valid for interaction queries.
	//
	// R Version :
	// ```R
	// fmMatcherAntAngleSmallerThan(angle)
	// ```
	//
	// @return a Matcher that matches when two <Ant> are facing the same
	// direction with <angle> threshold.
	static Ptr AntAngleSmallerThan(double angle);

	// Matches an angle between two Ants
	// @angle the angle to be greater (in radians).
	//
	// Matches that the angle between two Ants is greater than
	// <angle>, only valid for interaction queries.
	//
	// R Version :
	// ```R
	// fmMatcherAntAngleGreaterThan(angle)
	// ```
	//
	// @return a Matcher that matches when two <Ant> are not facing the
	// same direction with <angle> threshold.
	static Ptr AntAngleGreaterThan(double angle);

	// opaque pointer to implementation
	typedef std::shared_ptr<priv::Matcher> PPtr;

	// Matches an <InteractionType>
	// @type1 the first AntShapeTypeID to match
	// @type2 the second AntShapeTypeID to match
	//
	// Matches interactions with the specified type. Matches
	// `(type1,type2)` and `(type2,type1)` interactions.
	//
	// R Version :
	// ```R
	// fmMatcherInteractionType(type1,type2)
	// ```
	//
	// @return a Matcher that matches a given <InteractionType> or its
	// opposite.
	static Ptr InteractionType(AntShapeTypeID type1,
	                           AntShapeTypeID type2);


private:
	friend class Query;
	friend std::ostream & ::operator<<(std::ostream & out,
	                                   const fort::myrmidon::Matcher & m);

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
