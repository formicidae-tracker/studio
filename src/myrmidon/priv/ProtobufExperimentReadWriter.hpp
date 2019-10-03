#pragma once

#include "ExperimentReadWriter.hpp"

#include "TrackingDataDirectory.hpp"

#include "ForwardDeclaration.hpp"

namespace fort {

namespace myrmidon {

namespace pb {
class Experiment;
class TrackingDataDirectory;
class AntMetadata;
class Identification;
class FramePointer;
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

	static void LoadAnt(Experiment & e, const fort::myrmidon::pb::AntMetadata & pb);

	static void SaveAnt(fort::myrmidon::pb::AntMetadata & pb, const Ant & a);




	static void LoadIdentification(Experiment & e, const AntPtr & a,
	                               const fort::myrmidon::pb::Identification & pb);

	static void SaveIdentification(fort::myrmidon::pb::Identification & pb,
	                               const Identification & ident);


	static FramePointerPtr LoadFramePointer(const fort::myrmidon::pb::FramePointer & pb);

	static void SaveFramePointer(fort::myrmidon::pb::FramePointer & pb,
	                             const FramePointer & fp);

};

} // namespace priv

} // namespace myrmidon

} // namespace fort
