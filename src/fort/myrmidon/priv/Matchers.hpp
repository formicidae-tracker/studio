#pragma once

#include <fort/myrmidon/Types.hpp>

#include "Identifier.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

class Matcher {
public:
	typedef std::shared_ptr<Matcher> Ptr;

	static Ptr And(const std::vector<Ptr> & matcher);

	static Ptr Or(const std::vector<Ptr> & matcher);

	static Ptr AntIDMatcher(AntID ID);

	static Ptr AntColumnMatcher(const std::string & name, const AntStaticValue & value);

	static Ptr AntDistanceSmallerThan(double distance);

	static Ptr AntDistanceGreaterThan(double distance);

	static Ptr AntAngleGreaterThan(double angle);

	static Ptr AntAngleSmallerThan(double angle);

	static Ptr InteractionType(AntShapeTypeID type1,
	                           AntShapeTypeID type2);

	virtual void SetUpOnce(const ConstAntByID & ants) = 0;

	virtual void SetUp(const IdentifiedFrame::Ptr & identifiedFrame,
	                   const CollisionFrame::Ptr & collisionFrame) = 0;

	virtual bool Match(fort::myrmidon::AntID ant1,
	                   fort::myrmidon::AntID ant2,
	                   const fort::myrmidon::InteractionTypes & types) = 0;

	virtual void Format(std::ostream & out) const = 0;

	virtual ~Matcher();
};

} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream & operator<<(std::ostream & out, const fort::myrmidon::priv::Matcher & m);
