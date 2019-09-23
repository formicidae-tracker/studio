#pragma once

#include <memory>

#include "Experiment.pb.h"
#include "../Ant.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class Ant {
public:
	typedef std::shared_ptr<Ant> Ptr;
	Ant(fort::myrmidon::pb::AntMetadata * ant);
	~Ant();

	const fort::myrmidon::pb::AntMetadata * Metadata() const {
		return d_metadata.get();
	}

	fort::myrmidon::Ant::ID ID() const {
		return d_metadata->id();
	}

	const std::string & FormatID() const {
		return d_ID;
	}

private:
	std::shared_ptr<fort::myrmidon::pb::AntMetadata> d_metadata;

	std::string d_ID;

};

} //namespace priv

} // namespace myrmidon

} // namespace fort
