#include "Ant.priv.hpp"

using namespace fort::myrmidion::priv;

Ant::Ant(fort::myrmidion::pb::AntMetadata * ant)
	: d_metadata(ant) {
}

Ant::~Ant() {
}

const fort::myrmidion::pb::AntMetadata * Ant::Metadata() const {
	return d_metadata.get();
}
