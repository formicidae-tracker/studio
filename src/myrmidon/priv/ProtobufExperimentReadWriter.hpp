#pragma once

#include "ExperimentReadWriter.hpp"

#include "TrackingDataDirectory.hpp"

#include "ForwardDeclaration.hpp"

#include <myrmidon/File.pb.h>

namespace fort {

namespace myrmidon {


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


	static Time LoadTime(const fort::myrmidon::pb::Time & pb, Time::MonoclockID mID);
	static void SaveTime(fort::myrmidon::pb::Time & pb, const Time & t);

	static void LoadSegmentIndexer(SegmentIndexer & si,
	                               const google::protobuf::RepeatedPtrField<fort::myrmidon::pb::TrackingSegment> & pb,
	                               Time::MonoclockID mID);

	static void SaveSegmentIndexer(google::protobuf::RepeatedPtrField<fort::myrmidon::pb::TrackingSegment> * pb,
	                               const SegmentIndexer & si);


	static MovieSegment::Ptr LoadMovieSegment(const fort::myrmidon::pb::MovieSegment & ms,
	                                          const fs::path & base);

	static void SaveMovieSegment(fort::myrmidon::pb::MovieSegment * pb, const MovieSegment::Ptr & ms,
	                             const fs::path & base);


	// Unmarshals a TrackingDataDirectory from a protobuf message
	// @pb the protobuf message to read from
	// @return <TrackingDataDirectory> contained in the message
	static TrackingDataDirectory LoadTrackingDataDirectory(const fort::myrmidon::pb::TrackingDataDirectory & pb,
	                                                       const fs::path & base);

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


};

} // namespace priv

} // namespace myrmidon

} // namespace fort
