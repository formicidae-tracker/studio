#pragma once

#include <variant>
#include <map>
#include <vector>

#include <Eigen/Core>

#include "Time.hpp"


#include "ForwardDeclaration.hpp"



namespace fort {
namespace myrmidon {

// The ID for a tag
//
// The ID for a tag. As explained in <ant_identification> myrmidon
// doesn't allows to access tracking tag data directly, but through
// the definition of <Ant> and <Identification>.
typedef uint32_t TagID;

// A named value
//
// AntStaticValue holds a value for a <named_values>.
typedef std::variant<bool,int32_t,double,std::string,Time> AntStaticValue;

// A List of 2D Vector
//
// A List of 2D Vector
typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d>> Vector2dList;

// ID for Ant virtual body parts
//
// ID to designate an <Ant> virtual body part. See
// <ant_interaction>
typedef uint32_t AntShapeTypeID;

// ID for manual measurements
//
// ID to designate a manual measurement made in the GUI
typedef uint32_t MeasurementTypeID;

// Mandatory measurement type for any <Experiment>
//
// Any <Experiment> have this Measurement type that cannot be removed.
const MeasurementTypeID HEAD_TAIL_MEASUREMENT_TYPE = 1;


// A list of Ant virtual shape part
typedef std::vector<std::pair<AntShapeTypeID,const Capsule>> TypedCapsuleList;

// Types for non-tracking data column
enum class AntMetadataType {
                            // A boolean
                            BOOL = 0,
                            // An integer
                            INT,
                            // a float
                            DOUBLE,
                            // a std::string
                            STRING,
                            // a <Time>
                            TIME,
};

// Represents a Measurement in mm at a given Time.
//
// Measurement in myrmidon are automatically converted to MM given the
// <Experiment> tag family and size, and the size of the tag measured
// in the image.
struct ComputedMeasurement {
	// A list of measurement
	typedef std::vector<ComputedMeasurement> List;
	// The <Time> of the Measurement
	Time   MTime;
	// the value in mm of the measurement
	double LengthMM;
	// the value of the measurement in pixels
	double LengthPixel;
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

// A PositionnedAnt gives an <Ant> position and orientation in a
// <IdentifiedFrame>
struct PositionedAnt {
	// Memory management issue with Eigen
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	// The position in the image.
	Eigen::Vector2d Position;
	// The angle in the image
	double          Angle;
	// The <AntID> this position refers to.
	AntID        ID;
};
// A List of <PositionedAnt>
typedef std::vector<PositionedAnt,Eigen::aligned_allocator<PositionedAnt>> PositionedAntList;

// A visual frame were <Ant> have been identified from their <TagID>
struct IdentifiedFrame {
	// A pointer to an IdentifiedFrame
	typedef std::shared_ptr<IdentifiedFrame>       Ptr;
	// A pointer to an IdentifiedFrame
	typedef std::shared_ptr<const IdentifiedFrame> ConstPtr;
	// The <Time> at which this frame was taken.
	Time              FrameTime;
	// The <Space> this frame belongs to
	SpaceID           Space;
	// The height of the frame
	size_t            Height;
	// The width of the frame
	size_t            Width;
	// The list of identified <Ant> in the frame
	PositionedAntList Positions;
	// optionnal Zones for ants
	std::vector<ZoneID> Zones;
	// Tests if the frame contains an <Ant>
	// @antID the <AntID> of the <Ant> to test for.
	//
	// @return `true` if <antID> is in <Positions>
	bool Contains(uint64_t antID) const;
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
	typedef std::shared_ptr<const CollisionFrame> ConstPtr;
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
	typedef std::shared_ptr<const AntTrajectory> ConstPtr;

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
	Eigen::Matrix<double,Eigen::Dynamic,4> Positions;
	// Reports zone of ant if asked.
	//
	// Optional vector of either size 0 or Data.rows(). O value means
	// currentlty not in a zone.
	std::vector<uint32_t>                  Zones;

	// End <Time> for this Trajectory
	//
	// @return a <Time> computed from <Start> and the <Positions>
	//         data.
	Time End() const;
};

// Defines a sub segment of a trajectory
struct AntTrajectorySegment {
	// The refering trajectory
	AntTrajectory::ConstPtr Trajectory;

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
	typedef std::shared_ptr<const AntInteraction> ConstPtr;

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
