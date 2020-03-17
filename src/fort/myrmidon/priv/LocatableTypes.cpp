#include "LocatableTypes.hpp"


namespace fort {
namespace myrmidon {
namespace priv {


bool Identifiable::Comparator::operator()(const Identifiable & a ,
                                          const Identifiable & b) const {
	return a.URI() < b.URI();
}

bool Identifiable::Comparator::operator()(const std::shared_ptr<Identifiable> & a ,
                                          const std::shared_ptr<Identifiable> & b) const {
	return a->URI() < b->URI();
}

} //namespace priv
} //namespace myrmidon
} //namespace fort
