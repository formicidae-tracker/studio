#pragma once

#include <memory>

#include "Experiment.pb.h"

#include "Ant.priv.hpp"


namespace fort {
namespace myrmidion {
namespace priv {


class Experiment {
public :
	typedef std::unique_ptr<Experiment> Ptr;

	static Ptr Open(const std::string & filename);

	void Save(const std::string & filename) const;

	fort::myrmidion::pb::Experiment d_experiment;

	std::vector<Ant::Ptr> d_ants;
};

} //namespace priv

} //namespace myrmidion

} //namespace fort
