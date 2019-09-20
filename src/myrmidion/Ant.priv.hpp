#pragma once

#include <memory>

#include "Experiment.pb.h"
#include "Ant.hpp"

namespace fort {

namespace myrmidion {

namespace priv {

class Ant {
public:
	typedef std::shared_ptr<Ant> Ptr;
	Ant(fort::myrmidion::pb::AntMetadata * ant);
	~Ant();

	const fort::myrmidion::pb::AntMetadata * Metadata() const;

	fort::myrmidion::Ant::ID ID() const {
		return d_metadata->id();
	}

private:
	std::shared_ptr<fort::myrmidion::pb::AntMetadata> d_metadata;


};

} //namespace priv

} // namespace myrmidion

} // namespace fort
