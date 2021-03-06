#pragma once

#include <fort/tags/fort-tags.h>

#include <fort/myrmidon/Time.pb.h>
#include <fort/myrmidon/AntDescription.pb.h>
#include <fort/myrmidon/Experiment.pb.h>
#include <fort/myrmidon/TrackingDataDirectory.pb.h>
#include <fort/myrmidon/TagCloseUpCache.pb.h>
#include <fort/myrmidon/Shapes.pb.h>
#include <fort/myrmidon/Zone.pb.h>
#include <fort/myrmidon/Space.pb.h>

#include <fort/myrmidon/Time.hpp>
#include <fort/myrmidon/Color.hpp>

#include <fort/myrmidon/priv/ForwardDeclaration.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/priv/Ant.hpp>
#include <fort/myrmidon/priv/Shape.hpp>

#include <Eigen/Core>
#include <fort/myrmidon/Vector2d.pb.h>

namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

// Class to perform IO from/to protobuf
//
// This class only conatins static methods to convert private objects
// to and from protobuf messages.
//
// Methods reads from protobuf, uses const reference to corresponding
// protobuf messages.
//
// Methods that saves to protobuf, uses pointers to the coreesponding
// protobuf messages.
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
	static void LoadIdentification(const ExperimentPtr & e,
	                               const priv::AntPtr & a,
	                               const pb::Identification & pb);

	// Saves an Identification to protobuf message
	//
	// @pb the message to save to
	// @ident the <Identification> to save to
	static void SaveIdentification(pb::Identification * pb,
	                               const IdentificationConstPtr & ident);


	static Color LoadColor(const pb::Color & pb);

	static void  SaveColor(pb::Color * pb, const Color & c);

	static Ant::DisplayState LoadAntDisplayState(pb::AntDisplayState pb);

	static pb::AntDisplayState  SaveAntDisplayState(Ant::DisplayState s);


	static AntStaticValue LoadAntStaticValue(const pb::AntStaticValue & pb);

	static void SaveAntStaticValue(pb::AntStaticValue * pb, const AntStaticValue & value);


	// Loads an Ant from a protobuf message
	//
	// @e the <priv::Experiment> that will own the Ant
	// @pb the serialized data
	static void LoadAnt(const ExperimentPtr & e, const pb::AntDescription & pb);

	// Saves an Ant to a protobuf message
	//
	// @pb the message to save to
	// @a the <priv::Ant> to save
	static void SaveAnt(pb::AntDescription * pb, const AntConstPtr & a);

	// Converts a Family from a protobuf enum
	//
	// @pb the protobuf enum value
	// @return a <fort::tags::Family> enum value
	static tags::Family LoadFamily(const pb::TagFamily & pb);

	// Converts a Family to a protobuf enum
	//
	// @f the <fort::tags::Family> enum value
	// @return a corresponding pbValue
	static pb::TagFamily SaveFamily(const tags::Family f);

	// Loads a Measurement from a message
	//
	// @pb the message to read from
	// @return a <Measurement> from the message
	static MeasurementConstPtr LoadMeasurement(const pb::Measurement & pb);

	// Saves a Measurement to a message
	//
	// @pb the message to save to
	// @m the <Measurement> to save
	static void SaveMeasurement(pb::Measurement * pb, const MeasurementConstPtr & m);


	static void  LoadZone(const SpacePtr & space,
	                      const pb::Zone & pb);

	static void SaveZone(pb::Zone * pb, const ZoneConstPtr & zone);

	static void LoadSpace(const ExperimentPtr & e,
	                      const pb::Space & pb,
	                      bool loadTrackingDataDirectory = true);

	static void SaveSpace(pb::Space * pb,
	                      const SpaceConstPtr & space);


	// Loads an Experiment from a protobuf message
	//
	// @e the empty <priv::Experiment> to load data to
	// @pb the <pb::Experiment> protobuf message to read from
	static void LoadExperiment(const ExperimentPtr & e,
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
	                                         const std::string & parentURI,
	                                         Time::MonoclockID monoID);

	// Save a FrameReference to a message
	//
	// @pb the message to save to
	// @ref teh <priv::FrameReference> to save
	static void SaveFrameReference(pb::TimedFrame * pb,
	                               const FrameReference & ref);

	// Loads a TrackingIndex::Segment from a message
	//
	// @pb the protobuf message field to load from
	// @parentURI the URI of the parent <TrackingDataDirectory>
	// @monoID the <Time::MonoclockID> associated with the parent <TrackingDataDirectory>
	// @return the segment in the message
	static  TrackingDataDirectory::TrackingIndex::Segment
	LoadTrackingIndexSegment(const pb::TrackingSegment & pb,
	                         const std::string & parentURI,
	                         Time::MonoclockID monoID);

	// Saves a TrackingIndex to a message
	//
	// @pb the protobuf message field to save to
	// @si a <TrackingDataDirectory::TrackingIndex::Segment> to save
	static void SaveTrackingIndexSegment(pb::TrackingSegment * pb,
	                                     const TrackingDataDirectory::TrackingIndex::Segment & si);

	// Loads a MovieSegment from a message
	//
	// @pb the message to read from
	// @parentAbsoluteFilePath the absolute path to the parent directory
	// @parentURI the URI of the parent
	// @monoID the <Time::MonoclockID> associated with the parent
	// @return the <MovieSegment> in the message
	static MovieSegmentPtr LoadMovieSegment(const fort::myrmidon::pb::MovieSegment & pb,
	                                        const fs::path & parentAbsoluteFilePath,
	                                        const std::string & parentURI);

	// Saves a MovieSegment to a message
	//
	// @pb the destination message
	// @ms the <MovieSegment> to save
	// @parentAbsoluteFilePath the absolute path to the parent TrackingDataDirectory
	static void SaveMovieSegment(fort::myrmidon::pb::MovieSegment * pb,
	                             const MovieSegmentConstPtr & ms,
	                             const fs::path & parentAbsoluteFilePath);


	// Loads a TagCloseUp from a message
	//
	// @pb the message to read from
	// @absoluteBasedir the actual directory containing the close-ups images
	// @resolver a function that resolves <FrameID> to actual
	//           <FrameReference> for the collection of close-up
	//           contained in absoluteBaseDir.
	static TagCloseUpConstPtr LoadTagCloseUp(const pb::TagCloseUp & pb,
	                                         const fs::path & absoluteBasedir,
	                                         std::function<FrameReference (FrameID)> resolver);

	// Saves a TagCloseUp from a message
	//
	// @pb the message to save to
	// @tcu the TagCloseUp to save
	// @absoluteBasedir the actual directory containing the close-ups images
	static void SaveTagCloseUp(pb::TagCloseUp * pb,
	                           const TagCloseUpConstPtr & tcu,
	                           const fs::path & absoluteBasedir);


	// Loads a Capsule from a message
	//
	// @pb the protobuf message to read from
	// @return a <Capsule::Ptr> initialized with the message data
	static Capsule LoadCapsule(const pb::Capsule & pb);

	// Saves a Capsule to a message
	//
	// @pb the protobuf message to save to
	// @capsule the <Capsule> to save to
	static void SaveCapsule(pb::Capsule * pb,const Capsule & capsule);


	static CirclePtr LoadCircle(const pb::Circle & pb);

	static void SaveCircle(pb::Circle * pb, const CircleConstPtr & circle);

	static PolygonPtr LoadPolygon(const pb::Polygon & pb);

	static void SavePolygon(pb::Polygon * pb, const PolygonConstPtr & polygon);

	static Shape::Ptr LoadShape(const pb::Shape & pb);

	static void SaveShape(pb::Shape * pb, const Shape::ConstPtr & shape);


};

} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
