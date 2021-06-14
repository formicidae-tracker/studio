#pragma once

#include <variant>
#include <map>
#include <vector>

#include <Eigen/Core>

#include <fort/time/Time.hpp>


#include "ForwardDeclaration.hpp"

/**
 * the namespace for all the FORmicidae Tracker libraries
 */

namespace fort {

/**
 * FORT post-processing namespace
 *
 */

namespace myrmidon {

/**
 * The ID for a tag
 *
 * * Python: TagID translates to an `int`
 * * R: TagID translates to an `integer`
 *
 * The identifier for a tag, which relates to Ant using
 * Identification.
 */
typedef uint32_t TagID;


/**
 * The ID for an Ant.
 *
 * * Python: AntID translates to an `int`
 * * R: AntID translates to an `integer`
 *
 * Ant are uniquely identified within an Experiment with an AntID,
 * which is at least `1`. `0` is an invalid AntID.
 */
typedef uint32_t AntID;

/**
 * The ID for a Space.
 *
 * * Python: SpaceID translates to an `int`
 * * R: SpaceID translates to an `integer`
 *
 * Space are uniquely identified within an Experiment with a SpaceID,
 * which is at least `1`. `0` is an invalid SpaceID.
 */
typedef uint32_t SpaceID;

/**
 * The ID for a Zone.
 *
 * * Python: ZoneID translates to an `int`
 * * R: ZoneID translates to an `integer`
 *
 * Zone are uniquely identified within a Space with a ZoneID, which is
 * at least `1`. `0` is an invalid/undefined Zone.
 */
typedef uint32_t ZoneID;

/**
 * C++ type for named values.
 *
 * * Python: any object that is either a `bool`, an `int`, a `float`,
 *           a `str` or a `py_fort_myrmidon.Time`.
 * * R: any S expression that is either a `logical`, an `integer`, a
 *      `numeric`, a `character` or a `fmTime`.
 *
 * A c++ type that can hold only one of any #AntMetadataType.
 */
typedef std::variant<bool,int32_t,double,std::string,Time> AntStaticValue;

/**
 * A List of 2D Vector.
 *
 */
typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d>> Vector2dList;

/**
 * The ID for Ant virtual body parts
 *
 * * Python: AntShapeTypeID translates to an `int`
 * * R: AntShapeTypeID translates to an `integer`
 *
 * Uniquely identifies an Ant shape type in an Experiment, from
 * `1`. `0` is an invalid value.
 */
typedef uint32_t AntShapeTypeID;


/**
 * The ID for Ant manual measurement types
 *
 * * Python: MeasurementTypeID translates to an `int`
 * * R: MeasurementTypeID translates to an `integer`
 *
 * Uniquely identifies an Ant measurement type in an Experiment, from
 * `1`. `0` is an invalid value. The value `1` always refers to the
 * valid MeasurementTypeID #HEAD_TAIL_MEASUREMENT_TYPE.
 */
typedef uint32_t MeasurementTypeID;

/**
 * The head-tail Measurement type.
 *
 * This Measurement type is always define for any Experiment and
 * cannot be deleted. However, it can be renamed.
 */
const MeasurementTypeID HEAD_TAIL_MEASUREMENT_TYPE = 1;

/** A list of Ant virtual shape part
 *
 * * Python: a `list` of `tuple` containing an `integer` and a `py_fort_myrmidon.Capsule`. For example:
 * ```Python
 * shapeList = [(1,py_fort_myrmidon.Capsule((0,0),(0,1),0.5,0.5))]
 * ```
 * * R: \todo define me
 *
 */
typedef std::vector<std::pair<AntShapeTypeID,const Capsule>> TypedCapsuleList;

/**
 * AntMetaDataType enumerates possible type for AntStaticValue
 */
enum class AntMetaDataType {
                            /** A boolean */
                            BOOL = 0,
                            /** An integer */
                            INT,
                            /** a float */
                            DOUBLE,
                            /** a std::string */
                            STRING,
                            /** a Time */
                            TIME,
};

/**
 * Represents a Measurement in mm at a given Time.
 *
 * * Python: a `py_fort_myrmidon.ComputedMeasurement` object with read-only properties `Time`, `LengthMM` and `LengthPixel`.
 * * R: see Query::ComputeMeasurementFor
 *
 * Measurement in myrmidon are automatically converted to MM given the
 * Experiment tag family and size, and the size of the tag measured
 * in the image.
 */
struct ComputedMeasurement {
	/** A list of measurement */
	typedef std::vector<ComputedMeasurement> List;
	/** The Time of the Measurement */
	fort::Time Time;
	/** the value in mm of the measurement */
	double     LengthMM;
	/** the value of the measurement in pixels */
	double     LengthPixel;
};
// Statistics about a <TagID> in the experiment.
struct TagStatistics {
	// A map of <TagStatistics> indexed by <TagID>
	typedef std::map<TagID,TagStatistics>   ByTagID;
	// A vector of count.
	typedef Eigen::Matrix<uint64_t,Eigen::Dynamic,1> CountVector;
	// Designating each index of <CountVector>
	enum CountHeader {
	    // Number of time the <TagID> was seen in the <Experiment>
		TOTAL_SEEN        = 0,
		// Number of time the <TagID> was seen multiple time in the same Frame.
		MULTIPLE_SEEN     = 1,
		// Number of time their was a gap less than 500 milliseconds were the tracking was lost.
		GAP_500MS         = 2,
		// Number of time their was a gap less than 1 second were the tracking was lost.
		GAP_1S            = 3,
		// Number of time their was a gap less than 10 seconds were the tracking was lost.
		GAP_10S           = 4,
		// Number of times their was a gap less than 1 minute were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_1M            = 5,
		// Number of times their was a gap less than 10 minutes were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_10M           = 6,
		// Number of times their was a gap less than 1 hour were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_1H            = 7,
		// Number of times their was a gap less than 10 hours were the tracking was lost. Innacurate if there are more than one <Space> in the experiment.
		GAP_10H           = 8,
		// Number of times their was a gap of more than 10 hours were the tracking was lost. If using multiple space in an experiment, consider only smaller gap, and add all columns from <GAP_1M> up to this one to consider only gap bigger than 10S.
		GAP_MORE          = 9,
	};

	// The <TagID> this statistics refers too
	TagID       ID;
	// The first <Time> in the <Experiment> this <TagID> was detected.
	Time        FirstSeen;
	// The last <Time> in the <Experiment> this <TagID> was detected.
	Time        LastSeen;
	// Counts were the tag was seen
	CountVector Counts;
};

// A visual frame were <Ant> have been identified from their <TagID>
struct IdentifiedFrame {
	// A pointer to an IdentifiedFrame
	typedef std::shared_ptr<IdentifiedFrame>       Ptr;

	// A Matrix of posi
	typedef Eigen::Matrix<double,Eigen::Dynamic,5,Eigen::RowMajor> PositionMatrix;

	// The <Time> at which this frame was taken.
	Time              FrameTime;
	// The <Space> this frame belongs to
	SpaceID           Space;
	// The height of the frame
	size_t            Height;
	// The width of the frame
	size_t            Width;
	// The list of identified <Ant> in the frame
	PositionMatrix    Positions;

	// Tests if the frame contains an <Ant>
	// @antID the <AntID> of the <Ant> to test for.
	//
	// @return `true` if <antID> is in <Positions>
	bool Contains(uint64_t antID) const;

	std::tuple<AntID,const Eigen::Ref<const Eigen::Vector3d>,ZoneID> At(size_t index) const;
};

// Designates an interaction between two <Ant>
//
// Designates an interaction between two <Ant>, using their
// <AntID>. InteractionID are always constructed such as the first ID
// is strictly smaller than the second ID, so it ensures uniqueness of
// the InteractionID to reports interactions.
typedef std::pair<AntID,AntID>                   InteractionID;

// Designates list of interaction type for an interaction.
//
// Designates an interaction type for an interaction. Each line
// represent a colliding capsules type. First column the first ant and
// the second column the second ant.
typedef Eigen::Matrix<uint32_t,Eigen::Dynamic,2> InteractionTypes;


// Defines an interaction between two <Ant> ponctual in <Time>
struct Collision {
	// The <AntID> of the two Ants interacting.
	//
	// The <AntID> of the two Ants interacting. Please note that the
	// constraint `IDS.first < IDs.second` is always maintained to
	// ensure uniqueness of IDs for <AntInteraction>.
	InteractionID                IDs;
	// Reports all virtual <AntShapeTypeID> interacting between the two Ants.
	InteractionTypes             Types;
	// Reports the <Zone> where the interaction happened.
	//
	// Reports the <Zone> where the interaction happened, the
	// corresponding <Space> is reported in <CollisionFrame>. 0 means
	// the default zone.
	ZoneID                       Zone;
};

// Reports all <Collision> happening at a given time.
struct CollisionFrame {
	// A pointer to a CollisionFrame
	typedef std::shared_ptr<CollisionFrame> Ptr;
	// The <Time> when the interaction happens
	Time                   FrameTime;
	// Reports the <Space> this frame is taken from
	SpaceID                Space;
	// The <Collision> happenning at <FrameTime>
	std::vector<Collision> Collisions;

};

// Defines a trajectory for an <Ant>
struct AntTrajectory {
	// A pointer to the trajectory
	typedef std::shared_ptr<AntTrajectory> Ptr;

	// Reports the <AntID> of the <Ant> this trajectory refers to.
	AntID   Ant;
	// Reports the <Space> this trajectory is taking place.
	SpaceID Space;
	// Reports the starting <Time> of this trajectory.
	//
	// Reports the starting <Time> of this trajectory. <Positions>
	// first column are second offset from this time.
	Time    Start;
	// Reports the time and position in the frame.
	//
	// Reports the time and position in the frame.
	//
	// * first column: offset in second since <Start>
	// * second and third column: X,Y position in the image
	// * fourth column: Angle in ]-π,π], in trigonometric
	//   orientation. As in images Y axis points bottom, positove
	//   angle appears clockwise.
	// * fith column: the zone of the ant
	Eigen::Matrix<double,Eigen::Dynamic,5> Positions;

	// End <Time> for this Trajectory
	//
	// @return a <Time> computed from <Start> and the <Positions>
	//         data.
	Time End() const;
};

// Defines a sub segment of a trajectory
struct AntTrajectorySegment {
	// The refering trajectory
	AntTrajectory::Ptr Trajectory;

	// The starting index of the segment in the referring trajectory.
	size_t Begin;
	// The index after the last index in the referring trajectory.
	size_t End;

	// Optionally report the mean trajectory.
	std::unique_ptr<Eigen::Vector3d> Mean;
};




// Defines an interaction between two Ants
struct AntInteraction {
	// A pointer to the interaction structure
	typedef std::shared_ptr<AntInteraction> Ptr;

	// The IDs of the two <Ant>.
	//
	// The ID of the two <Ant>. Always reports `IDs.first <
	// IDs.second`.
	InteractionID                      IDs;
	// Virtual shape body part that were in contact.
	//
	// Virtual shape body part that were in contact during the
	// interaction.
	InteractionTypes                  Types;
	// Reports the <AntTrajectory> of each Ant.
	//
	// Reports the <AntTrajectory> of each Ant during the
	// interaction. The Trajectory are truncated to the interaction
	// timing.
	std::pair<AntTrajectorySegment,
	          AntTrajectorySegment>    Trajectories;
	// Reports the <Time> the interaction starts
	Time                               Start;
	// Reports the <Time> the interaction ends
	Time                               End;
	// Reports the <SpaceID> where the interaction happend
	SpaceID                            Space;

};


// Reports information about a tracking data directory.
struct TrackingDataDirectoryInfo {
	// The URI used in the GUI to designate the tracking data directory
	std::string URI;
	// The absolute filepath on the directory on the system
	std::string AbsoluteFilePath;
	// The number of Frames in this directory
	uint64_t    Frames;
	// The first frame <Time>
	Time        Start;
	// The last frame <Time>
	Time        End;
};

// Reports global tracking data stats for a <Space>
struct SpaceDataInfo {
	// The URI used to designate the <Space>
	std::string URI;
	// The name of the <Space>
	std::string Name;
	// The number of frame in the <Space>
	uint64_t    Frames;
	// The first <Time> present in the <Space>
	Time        Start;
	// The last <Time> present in the <Space>
	Time        End;

	// Infos for all tracking data directories, ordered in <Time>
	std::vector<TrackingDataDirectoryInfo> TrackingDataDirectories;
};

// Reports global tracking data stats for an <Experiment>
struct ExperimentDataInfo {
	// The number of tracked frame in the <Experiment>
	uint64_t Frames;
	// The <Time> of the first tracked frame
	Time     Start;
	// the <Time> of the last tracked frame
	Time     End;

	// Data infos for all <Space>
	std::map<SpaceID,SpaceDataInfo> Spaces;
};

// Formats a TagID to convention format
// @tagID the <TagID> to format
//
// @return <tagID> formatted to the myrmidon convetion for <TagID>.
std::string FormatTagID(TagID tagID);

}
}


// C++ Formatting operator for AntStaticValue
// @out the <std::ostream> to format the value to
// @v the <fort::myrmidon::AntStaticValue> to format
//
// @return a reference to <out>
std::ostream & operator<<(std::ostream & out, const fort::myrmidon::AntStaticValue & v);
