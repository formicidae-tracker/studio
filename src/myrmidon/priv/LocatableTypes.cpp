#include "LocatableTypes.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

bool Identifiable::operator<(const Identifiable & other) {
	return URI().generic_string() < other.URI().generic_string();
}

} //namespace priv
} //namespace myrmidon
} //namespace fort
