#pragma once

#include <myrmidon/Time.pb.h>
#include <myrmidon/AntMetadata.pb.h>
#include <myrmidon/Experiment.pb.h>

#include <myrmidon/Time.hpp>

#include <myrmidon/priv/ForwardDeclaration.hpp>


namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

class IOUtils {
public:

	static Time LoadTime(const pb::Time & pb, Time::MonoclockID mID);

	static void SaveTime(pb::Time * pb, const Time & t);

	// Loads an Identification from protobuf message
	//
	// @e the <priv::Experiment> that owns the <Identification> and the <priv::Ant>
	// @a the <priv::Ant> the <Identification> points to.
	// @pb the message to read from
	static void LoadIdentification(Experiment & e,
	                               const priv::AntPtr & a,
	                               const pb::Identification & pb);

	// Saves an Identification to protobuf message
	//
	// @pb the message to save to
	// @ident the <Identification> to save to
	static void SaveIdentification(pb::Identification * pb,
	                               const IdentificationConstPtr & ident);


	static CapsulePtr LoadShape(const pb::Capsule & pb);
	static void SaveShape(pb::Capsule * pb,const CapsulePtr & capsule);


	// Loads an Ant from a protobuf message
	//
	// @e the <priv::Experiment> that will own the Ant
	// @pb the serialized data
	static void LoadAnt(Experiment & e, const pb::AntMetadata & pb);

	// Saves an Ant to a protobuf message
	//
	// @pb the message to save to
	// @a the <priv::Ant> to save
	static void SaveAnt(pb::AntMetadata * pb, const AntConstPtr & a);






	// Loads an Experiment from a protobuf message
	//
	// @e the empty <priv::Experiment> to load data to
	// @pb the <pb::Experiment> protobuf message to read from
	static void LoadExperiment(Experiment & e,
	                           const pb::Experiment & pb);

	// Saves an Experiment to a protobuf message
	//
	// @pb the <pb::Experiment>  message
	// @e the <priv::Experiment> to save
	static void SaveExperiment(pb::Experiment * pb, const priv::Experiment & e);







	// static void LoadSegmentIndexer(TrackingDataDirectory::TrackingIndexer & si,
	//                                const google::protobuf::RepeatedPtrField<fort::myrmidon::pb::TrackingSegment> & pb,
	//                                Time::MonoclockID mID);

	// static void SaveSegmentIndexer(google::protobuf::RepeatedPtrField<fort::myrmidon::pb::TrackingSegment> * pb,
	//                                const TrackingDataDirectory::TrackingIndexer & si);


	// static MovieSegment::Ptr LoadMovieSegment(const fort::myrmidon::pb::MovieSegment & ms,
	//                                           const fs::path & base);

	// static void SaveMovieSegment(fort::myrmidon::pb::MovieSegment * pb, const MovieSegment::Ptr & ms,
	//                              const fs::path & base);


	// // Unmarshals a TrackingDataDirectory from a protobuf message
	// // @pb the protobuf message to read from
	// // @return <TrackingDataDirectory> contained in the message
	// static TrackingDataDirectory LoadTrackingDataDirectory(const fort::myrmidon::pb::TrackingDataDirectory & pb,
	//                                                        const fs::path & base);

	// // Saves a TrackingDataDirectory to a protobuf message
	// // @pb the protobuf message to save to
	// // @tdd the <TrackingDataDirectory> to save
	// static void SaveTrackingDataDirectory(fort::myrmidon::pb::TrackingDataDirectory & pb,
	//                                       const TrackingDataDirectory & tdd);


};

} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
