#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include "Types.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::Ant> implemenation
class Identification;
}


// Relates <TagID> to <Ant>
//
// An Identification relates a <TagID> to an <Ant>.
//
// ## Time definition
//
// Identification are bounded in <Time> in the range
// [<Start>,<End>[. An empty <Start> or <End> respectively means -∞ or
// +∞. Internally myrmidon ensure time validity of
// <Identification>. It means that:
//
// * Two Identification using the same <TagValue> cannot overlap in <Time>.
// * Two identification pointing to the same <Ant> cannot overlap in <Time>.
//
// ## Pose information
//
// Identification also contains geometric information on how the
// detected tag is related to the observed <Ant>. These are the
// translation and rotation of the <Ant>, in the tag reference frame.
//
// This information is either automatically generated from the
// measurement made in FORT Studio. Otherwise user can change this
// behavior by setting themselves this pose using
// <SetUserDefinedAntPose>. <ClearUserDefinedAntPose> can be used to
// revert to the internally computed pose.
//
// Note that any angle is measured in radians, with a standard
// mathematical convention. Since in image processing the y-axis is
// pointing from the top of the image to the bottom, positive angles
// appears clockwise.
//
class Identification {
public:
	// A pointer to an Identification
	typedef std::shared_ptr<Identification>       Ptr;
	// A const pointer to an Identification
	typedef std::shared_ptr<const Identification> ConstPtr;
	// A list of Identification
	typedef std::vector<Ptr>                      List;
	// A const list of Identification
	typedef std::vector<ConstPtr>                 ConstList;

	// Gets the TagID of this Identification
	//
	// @return the <TagID> used by this Identification
	TagID TagValue() const;


	// Sets the starting validity time for this Identification
	// @start the starting <Time> could be an empty pointer to
	//        represent -∞
	//
	// Sets the starting validity <Time> for this Identification. This
	// method will throw any <OverlappingIdentification> if such
	// modification will create any collision for the same <TagID> or
	// the same <Ant>. In such a case the boundaries remain unchanged.
	//
	// Identification are valid for [<Start>,<End>[
	void SetStart(const Time::ConstPtr & start);

	// Sets the ending validity time for this Identification
	// @end the endinf <Time> could be an empty pointer to
	//        represent +∞
	//
	// Sets the endibf validity <Time> for this Identification. This
	// method will throw any <OverlappingIdentification> if such
	// modification will create any collision for the same <TagID> or
	// the same <Ant>. In such a case the boundaries remain unchanged.
	//
	// Identification are valid for [<Start>,<End>[
	void SetEnd(const Time::ConstPtr & end);

	// Gets the starting validity time
	//
	// Identification are valid for [<Start>,<End>[
	//
	// @return the <Time> after which this Identification is
	//         valid. nullptr represents -∞
	Time::ConstPtr Start() const;

	// Gets the ending validity time
	//
	// Identification are valid for [<Start>,<End>[
	//
	// @return the <Time> after which this Identification is
	//         unvalid. nullptr represents +∞
	Time::ConstPtr End() const;

	// Gets the Ant position relatively to the tag center
	//
	// Gets the Ant position relatively to the tag center. This offset
	// is expressed in the tag reference frame.
	//
	// @return an <Eigen::Vector2d> of the <Ant> center relative to
	// the tag center.
	Eigen::Vector2d AntPosition() const;

	// Gets the Ant angle relatively to the tag rotation
	//
	// Gets the Ant position relatively to the tag center. This offset
	// is expressed in the tag reference frame.
	//
	// Angles use standard mathematical orientation. One has to
	// remember that the y-axis in image processing is pointing from
	// top to bottom, so when looking at the image, positive angle are
	// clockwise, which is the opposite of most mathematical drawing
	// when y is pointing from bottom to top.
	//
	// @return the angle in radian between the tag orientation and the
	//         ant orientation.
	double AntAngle() const;


	// Tests if Identification has a user defined pose
	//
	// @return `true` if the Identification has a user defined pose
	//         through <SetUserDefinedAntPose>
	bool HasUserDefinedAntPose() const;

	// Sets a user defined Ant pose
	// @antPosition the offset, from the tag center to the <Ant>
	//              center, expressed in the tag reference frame.
	// @antAngle the <Ant> angle, relative to the tag angle.
	//
	void SetUserDefinedAntPose(const Eigen::Vector2d & antPosition, double antAngle);

	// Clears any user defined pose.
	//
	// Clears any user defined pose for this Identification. Myrmidon
	// will re-compute the <Ant> pose from measurement made in FORT
	// Studio.
	void ClearUserDefinedAntPose();

	// An opaque pointer to implementation
	typedef std::shared_ptr<priv::Identification> PPtr;

	// Private implementation constructor
	// @pptr opaque pointer to implementation
	//
	// User cannot build Identification directly. They must be build
	// from <Experiment> and accessed from <Ant>
	Identification(const PPtr & pptr);

	// Private implementation downcaster
	//
	// @return the opaque private implementation
	const PPtr & ToPrivate() const;
private:

	PPtr d_p;
};

// Exception when two <Identification> overlaps in time.
//
// Two <Identification> overlaps in time if they have overlapping
// boundary and they either use the same <TagID> or targets the same
// <Ant>. This is an invariant condition that should never happen and
// modification that will break this invariant will throw this
// exception.
class OverlappingIdentification : public std::runtime_error {
public:
	// Default constructor
	// @a the first overlapping identification
	// @b the second overlapping identification
	OverlappingIdentification(const priv::Identification & a,
	                          const priv::Identification & b) noexcept;
private:
	static std::string Reason(const priv::Identification & a,
	                          const priv::Identification & b) noexcept;
};


} // namespace fort
} // namespace myrmidon
