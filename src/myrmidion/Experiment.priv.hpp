#pragma once

#include <memory>

#include "Experiment.pb.h"

#include "Ant.priv.hpp"


namespace fort {
namespace myrmidion {
namespace priv {

using namespace fort::myrmidion;

class Experiment {
public :
	typedef std::unique_ptr<Experiment> Ptr;

	static Ptr Open(const std::string & filename);

	void Save(const std::string & filename) const;



	void AddTrackingDataDirectory(const pb::TrackingDataDirectory & tdd);
	void RemoveRelativeDataPath(const std::string & path);

	const std::vector<std::string> & TrackingDataPath() const;

	//TODO Remove this helper method
	void AddAnt(const fort::myrmidion::pb::AntMetadata * md);
	const std::vector<Ant::Ptr> & Ants() const;

private:
	fort::myrmidion::pb::Experiment d_experiment;

	std::vector<Ant::Ptr> d_ants;
};

} //namespace priv

} //namespace myrmidion

} //namespace fort
