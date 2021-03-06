#include "Matchers.hpp"

#include "priv/Matchers.hpp"


namespace fort {
namespace myrmidon {

Matcher::Ptr Matcher::And(std::vector<Ptr> matchers) {
	std::vector<PPtr> pMatchers;
	pMatchers.reserve(matchers.size());
	for ( const auto & m : matchers ) { pMatchers.push_back(m->d_p); }
	return std::make_shared<Matcher>(priv::Matcher::And(pMatchers));
}

Matcher::Ptr Matcher::Or(std::vector<Ptr> matchers) {
	std::vector<PPtr> pMatchers;
	pMatchers.reserve(matchers.size());
	for ( const auto & m : matchers ) { pMatchers.push_back(m->d_p); }
	return std::make_shared<Matcher>(priv::Matcher::Or(pMatchers));
}

Matcher::Ptr Matcher::AntID(fort::myrmidon::AntID ID) {
	return std::make_shared<Matcher>(priv::Matcher::AntIDMatcher(ID));
}

Matcher::Ptr Matcher::AntColumn(const std::string & name,
                                       const AntStaticValue & value) {
	return std::make_shared<Matcher>(priv::Matcher::AntColumnMatcher(name,value));
}

Matcher::Ptr Matcher::AntDistanceSmallerThan(double distance) {
	return std::make_shared<Matcher>(priv::Matcher::AntDistanceSmallerThan(distance));
}

Matcher::Ptr Matcher::AntDistanceGreaterThan(double distance) {
	return std::make_shared<Matcher>(priv::Matcher::AntDistanceGreaterThan(distance));
}

Matcher::Ptr Matcher::AntAngleGreaterThan(double angle) {
	return std::make_shared<Matcher>(priv::Matcher::AntAngleGreaterThan(angle));
}

Matcher::Ptr Matcher::AntAngleSmallerThan(double angle) {
	return std::make_shared<Matcher>(priv::Matcher::AntAngleSmallerThan(angle));
}

Matcher::Ptr Matcher::InteractionType(AntShapeTypeID type1,
                                      AntShapeTypeID type2) {
	return std::make_shared<Matcher>(priv::Matcher::InteractionType(type1,type2));
}


} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Matcher & m) {
	return out << *m.d_p;
}
