#include "LocatableTypes.hpp"


namespace fort {
namespace myrmidon {
namespace priv {


bool Identifiable::Comparator::operator()(const Identifiable & a , const Identifiable & b) {
	return a.URI() < b.URI();
}

} //namespace priv
} //namespace myrmidon
} //namespace fort
