#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include "Types.hpp"


namespace fort {
namespace myrmidon {
class Identification;
class Experiment;

}
}

// Formats an Identification to an std::ostream
// @out the stream to format to
// @identification the <fort::myrmidon::Identification> to format
//
// @return a reference to <out>
std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Identification & identification);


namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::Ant> implemenation
class Identification;
}


/**
 * Relates TagID to Ant
 *
 * An Identification relates a ::TagID to an Ant, with Time validity
 * data and geometric data.
 *
 * Time validy
 * ===========
 *
 * Identification are bounded in Time in the range
 * [Start(),End()[. Start() can be Time::SinceEver() and End() can be
 * Time::ForEver(). These value are modifiable with SetStart() and
 * SetEnd().
 *
 * Internally `fort-myrmidon` ensure time validity of
 * Identification. It means that:
 *
 * * Two Identification using the same TagValue cannot overlap in Time.
 * * Two Identification pointing to the same Ant cannot overlap in Time.
 *
 * Pose information
 * ================
 *
 * Identification also contains geometric information on how the
 * detected tag is related to the observed Ant. These are the
 * translation and rotation of the Ant, in the tag coordinate reference.
 *
 * This information is either automatically generated from the manual
 * measurement #HEAD_TAIL_MEASUREMENT_TYPE made in
 * `fort-studio`. Alternatively, users can override this behavior by
 * setting themselves this pose using
 * SetUserDefinedAntPose(). ClearUserDefinedAntPose() can be used to
 * revert to the internally computed pose.
 *
 * \note Any angle is measured in radians, with a standard
 * mathematical convention. Since in image processing the y-axis is
 * pointing from the top of the image to the bottom, positive angles
 * appears clockwise.
 */
class Identification {
public:
	/** A pointer to an Identification */
	typedef std::shared_ptr<Identification>       Ptr;

	/**
	 * A list of Identification
	 *
	 * * Python: a `list` of `py_fort_myrmidon.Identification`
	 * * R: a `slist` of `Rcpp_fmIdentification`
	 */
	typedef std::vector<Ptr>                      List;

	/**
	 *  Gets the ::TagID of this Identification
	 *
	 * * Python: `TagValue: int` read-only property of `py_fort_myrmidon.Identification` objects
	 * * R:
	 * ```R
	 * fmIdentificationTagID <- function(identification) # returns an integer
	 * ```
	 *
	 * The associated TagID of an Identification is immuable.
	 *
	 * @return the ::TagID used by this Identification
	 */
	TagID TagValue() const;

	/*
	 * Gets the AntID of the targeted Ant
	 *
	 * * Python: `TargetAntID: int` read-only property of `py_fort_myrmidon.Identication` objects
	 * * R:
	 * ```R
	 * fmIdentificationTargetAntID <- function(identification) # returns an integer
	 * ```
	 * The targeted Ant of an Identification is immuable.
	 *
	 * @return the ::AntID of the targetted Ant
	 */
	AntID TargetAntID() const;

	/*
	 *  Sets the starting validity Time for this Identification
	 *
	 * * Python: `Start: py_fort_myrmidon.Time` property of `py_fort_myrmidon.Identification` objects
	 * * R:
	 * ```R
	 * fmIdentificationSetStart <- function(identification, start= fmTimeSinceEver() )
	 * ```
	 *
	 * @param start the starting Time. It can be Time::SinceEver().
	 *
	 * Sets the starting validity Time for this Identification,
	 * i.e. the first Time this Identification is valid
	 * (Identification are valid for [Start(),End()[)
	 *
	 * @throws OverlappingIdentification if start will make two
	 * Identification overlap in Time.
	 */
	void SetStart(const Time & start);

	/**
	 *  Sets the ending validity time for this Identification
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Identification.Start # read-write property
	 * ```
	 * * R:
	 * ```R
	 * fmIdentificationSetEnd(identification, start = fmTimeForever())
	 * ```
	 *
	 * @param end the ending Time. It can be Time::Forever().
	 *
	 * Sets the ending validity Time for this Identification, i.e. the
	 * first Time where this Identification becomes invalid
	 * (Identification are valid for [Start(),End()[).
	 *
	 *
	 * @throws OverlappingIdentification if end will make two
	 * Identification overlap in Time.
	 *
	 */
	void SetEnd(const Time & end);

	/**
	 *  Gets the starting validity time
	 *
	 * * Python: `Start: py_fort_myrmidon.Time` property of `py_fort_myrmidon.Identification` objects
	 * * R:
	 * ```R
	 * fmIdentificationStart <- function(identification)# returns a Rcpp_fmTime
	 * ```
	 *
	 * First Time where this Identification becomes valid.
	 * @return the Time where where this Identification becomes
	 *         valid. It can return Time::SinceEver()
	 */
	Time Start() const;

	/**
	 * Gets the ending validity time
	 *
	 * * Python: `End: py_fort_myrmidon.Time` property of `py_fort_myrmidon.Identification` objects
	 * * R:
	 * ```R
	 * fmIdentificationEnd <- function(identification)# returns a Rcpp_fmTime
	 * ```
	 *
	 * First Time where this Identification becomes unvalid.
	 * @return the first <Time> where this Identification becomes
	 *         unvalid. It can return Time::Forever()
	 */
	Time End() const;

	/**
	 * Gets the Ant center position relatively to the tag center.
	 *
	 * * Python: `AntPositon: nm.Matrix` read-only property of `py_fort_myrmidon.Identification` objects.
	 * * R:
	 * ```R
	 * fmIdentificationAntPosition<- function(identification) # returns a vector of two numerical
	 * ```
	 *
	 * Gets the Ant center position relatively to the tag center. This offset
	 * is expressed in the tag reference frame.
	 *
	 * @return an Eigen::Vector2d of the Ant center relative to
	 *         the tag center.
	 */
	Eigen::Vector2d AntPosition() const;

	/**
	 * Gets the Ant angle relatively to the tag rotation
	 *
	 * * Python: `AntAngle: float` read-only property of `py_fort_myrmidon.Identification` objects.
	 * * R:
	 * ```R
	 * fmIdentificationAntAngle<- function(identification) # returns a vector of two numerical
	 * ```
	 *
	 * Gets the Ant position relatively to the tag center. This offset
	 * is expressed in the tag reference frame.
	 *
	 * \note Angles use standard mathematical orientation. One has to
	 * remember that the y-axis in image processing is pointing from
	 * top to bottom, so when looking at the image, positive angle are
	 * clockwise, which is the opposite of most mathematical drawing
	 * when y is pointing from bottom to top.
	 *
	 * @return the angle in radian between the tag orientation and the
	 *         ant orientation.
	 */
	double AntAngle() const;


	/**
	 *  Indicates if Identification has a user defined pose.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Identification.HasUserDefinedPose(self) -> bool
	 * ```
	 * * R:
	 * ```R
	 * fmIdentificationHasUserDefinedPose <- function(identification) # returns a logical
	 * ```
	 *
	 * @return `true` if the Identification has a user defined pose
	 *         through SetUserDefinedAntPose()
	 */
	bool HasUserDefinedAntPose() const;

	/**
	 *  Sets a user defined Ant pose.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Identification.SetUserDefinedAntPose(self,antPosition: nm.Matrix,antAngle: float)
	 * ```
	 * * R:
	 * ```R
	 * fmIdentificationSetUserDefinedAntPose <- function(identification,antPosition = c(0.0,0.0),antAngle = 0.0)
	 * ```
	 *
	 * @param antPosition the offset, from the tag center to the Ant
	 *                    center, expressed in the tag reference frame.
	 * @param antAngle the Ant angle, relative to the tag angle.
	 *
	 */
	void SetUserDefinedAntPose(const Eigen::Vector2d & antPosition, double antAngle);

	/**
	 * Clears any user defined pose.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Identification.ClearUserDefinedAntPose(self)
	 * ```
	 * * R:
	 * ```R
	 * fmIdentificationClearUserDefinedAntPose <- function(identification)
	 * ```
	 *
	 * Clears any user defined pose for this
	 * Identification. `fort-myrmidon` will re-compute the Ant pose
	 * from #HEAD_TAIL_MEASUREMENT_TYPE measurement made in
	 * `fort-studio`.
	 */
	void ClearUserDefinedAntPose();

private:
	friend class Ant;
	friend class Experiment;
	friend std::ostream & ::operator<<(std::ostream &, const Identification&);

	// An opaque pointer to implementation
	typedef std::shared_ptr<priv::Identification> PPtr;

	PPtr d_p;

	// Private implementation constructor
	// @pptr opaque pointer to implementation
	//
	// User cannot build Identification directly. They must be build
	// from <Experiment> and accessed from <Ant>
	Identification(const PPtr & pptr);

	Identification & operator= (const Identification &) = delete;
	Identification(const Identification &) = delete;
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

	// virtual destructor
	virtual ~OverlappingIdentification();
private:
	static std::string Reason(const priv::Identification & a,
	                          const priv::Identification & b) noexcept;
};



} // namespace fort
} // namespace myrmidon
