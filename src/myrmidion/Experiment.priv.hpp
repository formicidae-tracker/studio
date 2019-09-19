#pragma once

#include <memory>

#include "Experiment.pb.h"



namespace fort {
namespace myrmidion {
namespace priv {


class Experiment {
public :
	typedef std::unique_ptr<Experiment> Ptr;

	static Ptr Open(const std::string & filename);

	void Save(const std::string & filename) const;

private:
	fort::myrmidion::pb::Experiment d_experiment;

	std::vector<fort::myrmidion::pb::AntMetadata> d_ants;
};

} //namespace priv

} //namespace myrmidion

} //namespace fort
