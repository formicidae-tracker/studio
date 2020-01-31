#pragma once

#include <myrmidon/Time.pb.h>
#include <myrmidon/AntMetadata.pb.h>
#include <myrmidon/Experiment.pb.h>
#include <myrmidon/TrackingDataDirectory.pb.h>

#include <myrmidon/Time.hpp>

#include <myrmidon/priv/ForwardDeclaration.hpp>
#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include <Eigen/Core>
#include <myrmidon/Vector2d.pb.h>

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

class IOUtils {
public:

	// Loads a Time from protobuf message
	//
	// @pb the message to read from
	// @mID the<Time::MonoclockID> to use if the message cotains a monotonic value
	// @return a <Time> oject, that may contain a monotonic value
	static Time LoadTime(const pb::Time & pb, Time::MonoclockID mID);

	// Saves a Time to a protobuf message
	//
	// @pb the message to save to
	// @t the <Time> to save
	static void SaveTime(pb::Time * pb, const Time & t);

	// Loads a Eigen::Vector2d from a message
	//
	// Loads a Eigen::Vector2d from a message. Note that we pass the
	// return value by reference to avoid memory alignement issues
	// with Eigen primitives.
	// @v the Eigen::Vector2d to load
	// @pb the protobuf message to load from
	static inline void LoadVector(Eigen::Vector2d & v, const pb::Vector2d & pb) {
		v << pb.x(),pb.y();
	}

	// Saves a Eigen::Vector2d to a message
	//
	// @pb the message to save to
	// @v the Eigen::Vector2d message to save
	static inline void SaveVector(pb::Vector2d * pb, const Eigen::Vector2d & v) {
		pb->set_x(v.x());
		pb->set_y(v.y());
	}

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


	// Loads a Capsule from a message
	//
	// @pb the protobuf message to read from
	// @return a <Capsule::Ptr> initialized with the message data
	static CapsulePtr LoadCapsule(const pb::Capsule & pb);

	// Saves a Capsule to a message
	//
	// @pb the protobuf message to save to
	// @capsule the <Capsule> to save to
	static void SaveCapsule(pb::Capsule * pb,const CapsuleConstPtr & capsule);


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


	// Loads a FrameReference from a message
	//
	// @pb the message to read from
	// @parentURI the URI of the parent <TrackingDataDirectory>
	// @monoID the <Time::MonoclockID> associated with the parent <TrackingDataDirectory>
	// @return a <FrameReference> contained in the message
	static FrameReference LoadFrameReference(const pb::TimedFrame & pb,
	                                         const fs::path & parentURI,
	                                         Time::MonoclockID monoID);

	// Save a FrameReference to a message
	//
	// @pb the message to save to
	// @ref teh <priv::FrameReference> to save
	static void SaveFrameReference(pb::TimedFrame * pb,
	                               const FrameReference & ref);

	// Loads a TrackingIndex from a message
	//
	// @si a <TrackingDataDirectory::TrackingIndex> to load
	// @pb the protobuf message field to load from
	// @parentURI the URI of the parent <TrackingDataDirectory>
	// @monoID the <Time::MonoclockID> associated with the parent <TrackingDataDirectory>
	static void LoadTrackingIndex(TrackingDataDirectory::TrackingIndex::Ptr & si,
	                              const google::protobuf::RepeatedPtrField<fort::myrmidon::pb::TrackingSegment> & pb,
	                              const fs::path & parentURI,
	                              Time::MonoclockID monoID);

	// Saves a TrackingIndex to a message
	//
	// @pb the protobuf message field to save to
	// @si a <TrackingDataDirectory::TrackingIndex> to save
	static void SaveTrackingIndex(google::protobuf::RepeatedPtrField<fort::myrmidon::pb::TrackingSegment> * pb,
	                              const TrackingDataDirectory::TrackingIndex::ConstPtr & si);


	static MovieSegmentPtr LoadMovieSegment(const fort::myrmidon::pb::MovieSegment & ms,
	                                        const fs::path & parentAbsoluteFilePath);

	static void SaveMovieSegment(fort::myrmidon::pb::MovieSegment * pb,
	                             const MovieSegmentConstPtr & ms);


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
