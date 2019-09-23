#pragma once

#include <memory>

#include "Experiment.pb.h"

#include "Ant.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

using namespace fort::myrmidon;

class Experiment {
public :
	typedef std::unique_ptr<Experiment> Ptr;

	static Ptr Open(const std::string & filename);

	void Save(const std::string & filename) const;


	void AddTrackingDataDirectory(const pb::TrackingDataDirectory & tdd);
	void RemoveRelativeDataPath(const std::string & path);

	std::vector<std::string> TrackingDataPath() const;

	//TODO Remove this helper method
	void AddAnt(fort::myrmidon::pb::AntMetadata * md);
	const std::vector<Ant::Ptr> & Ants() const;

private:
	fort::myrmidon::pb::Experiment d_experiment;

	std::vector<Ant::Ptr> d_ants;
};

} //namespace priv

} //namespace myrmidon

} //namespace fort
