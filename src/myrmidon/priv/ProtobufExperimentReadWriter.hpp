#pragma once

#include "ExperimentReadWriter.hpp"

#include "TrackingDataDirectory.hpp"

namespace fort {

namespace myrmidon {

namespace pb {
class Experiment;
class TrackingDataDirectory;
}

namespace priv {

class ProtobufReadWriter : public ExperimentReadWriter {
public:
	ProtobufReadWriter();
	virtual ~ProtobufReadWriter();

	virtual ExperimentPtr DoOpen(const std::filesystem::path & filename);

	virtual void DoSave(const Experiment & experiment, const std::filesystem::path & filename);

	static void LoadExperiment(Experiment & e,const fort::myrmidon::pb::Experiment & pb);

	static void SaveExperiment(fort::myrmidon::pb::Experiment & pb, const Experiment & e);

	static TrackingDataDirectory LoadTrackingDataDirectory(const fort::myrmidon::pb::TrackingDataDirectory & pb);

	static void SaveTrackingDataDirectory(fort::myrmidon::pb::TrackingDataDirectory & pb,
	                                      const TrackingDataDirectory & tdd);


};

} // namespace priv

} // namespace myrmidon

} // namespace fort
