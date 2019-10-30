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

// Saves Experiment using protocol buffer
//
// This <ExperimentReadWriter> read and saves data using protocol
// buffer.
class ProtobufReadWriter : public ExperimentReadWriter {
public:
	// Constructor
	ProtobufReadWriter();
	// Destructor
	virtual ~ProtobufReadWriter();

	// Implements DoOpen
	virtual ExperimentPtr DoOpen(const fs::path & filename);

	// Implements DoSave
	virtual void DoSave(const Experiment & experiment, const fs::path & filename);

	// Loads an Experiment from a protobuf message
	// @e the empty Experiment to load data to
	// @pb the protobuf message to read from
	static void LoadExperiment(Experiment & e,const fort::myrmidon::pb::Experiment & pb);

	// Saves an Experiment to a protobuf message
	// @pb the protobuf message
	// @e the <priv::Experiment> to save
	static void SaveExperiment(fort::myrmidon::pb::Experiment & pb, const Experiment & e);

	// Unmarshals a TrackingDataDirectory from a protobuf message
	// @pb the protobuf message to read from
	// @return <TrackingDataDirectory> contained in the message
	static TrackingDataDirectory LoadTrackingDataDirectory(const fort::myrmidon::pb::TrackingDataDirectory & pb);

	// Saves a TrackingDataDirectory to a protobuf message
	// @pb the protobuf message to save to
	// @tdd the <TrackingDataDirectory> to save
	static void SaveTrackingDataDirectory(fort::myrmidon::pb::TrackingDataDirectory & pb,
	                                      const TrackingDataDirectory & tdd);

	// Loads an Ant from a protobuf message
	// @e the <priv::Experiment> that will own the Ant
	// @pb the serialized data
	static void LoadAnt(Experiment & e, const fort::myrmidon::pb::AntMetadata & pb);

	// Saves an Ant to a protobuf message
	// @pb the message to save to
	// @a the <priv::Ant> to save
	static void SaveAnt(fort::myrmidon::pb::AntMetadata & pb, const Ant & a);

	// Loads an Identification from protobuf message
	// @e the <priv::Experiment> that owns the <Identification> and the <priv::Ant>
	// @a the <priv::Ant> the <Identification> points to.
	// @pb the message to read from
	static void LoadIdentification(Experiment & e, const AntPtr & a,
	                               const fort::myrmidon::pb::Identification & pb);

	// Saves an Identification to protobuf message
	// @pb the message to save to
	// @ident the <Identification> to save to
	static void SaveIdentification(fort::myrmidon::pb::Identification & pb,
	                               const Identification & ident);


	// Loads a FramePointer from protobuf message
	// @pb the message to save to
	// @return the <FramePointer>
	static FramePointerPtr LoadFramePointer(const fort::myrmidon::pb::FramePointer & pb);

	// Saves a FramePointer to protobuf message
	// @pb the message to save to
	// @fp the <FramePointer> to save to
	static void SaveFramePointer(fort::myrmidon::pb::FramePointer & pb,
	                             const FramePointer & fp);

};

} // namespace priv

} // namespace myrmidon

} // namespace fort
