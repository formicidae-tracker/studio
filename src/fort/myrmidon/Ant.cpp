#include "Ant.hpp"

#include <fort/myrmidon/priv/Ant.hpp>

namespace fort {
namespace myrmidon {


Ant::ID Ant::AntID() const {
	return d_p->AntID();
}

std::string Ant::FormattedID() const {
	return d_p->FormattedID();
}


const Color & Ant::DisplayColor() const {
	return d_p->DisplayColor();
}


} // namespace myrmidon
} // namespace fort
