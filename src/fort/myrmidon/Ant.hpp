#pragma once

#include <memory>

#include "Types.hpp"
#include "Color.hpp"
#include "Identification.hpp"
#include "ForwardDeclaration.hpp"
#include "Shapes.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::Ant> implemenation
class Ant;
} // namespace priv



/**
 * The main object of interest of any Experiment
 *
 * \headerfile fort/myrmidon/Ant.hpp
 *
 * Ant are the object of interest of an Experiment.
 *
 * Naming
 * ======
 *
 * Ant are uniquely identified by an AntID. By convention we use
 * decimal notation with up to two `0` prefix to display an ID, as
 * returned by FormattedID.
 *
 * Identification
 * ==============
 *
 * Instead of working directly with TagID `fort-myrmidon` uses
 * Identification to relates TagID to an Ant. An Ant could have
 * different Identification, allowing us to use different TagID to
 * refer to the same individual. `fort-myrmidon` ensures the following
 * invariant:
 *
 * * At any Time there is only a single TagID used to designate an
 *   Ant
 * * For any Ant, there only a single Identification that designate
 *   this Ant at any given Time. I.e. Identification::Start and
 *   Identification::End must not overlap for a given Ant.
 *
 * One would use IdentifiedAt() to obtain the TagID that identifies
 * an Ant at a given Time.
 *
 * Ant Virtual Shape
 * =================
 *
 * Each Ant has an associated virtual shape that is used to compute
 * instantaneous Collision detection ( Query::CollideFrame() ), or
 * timed AntInteraction ( Query::ComputeAntInteraction ). These shape
 * can be defined manually in `fort-studio` or programmatically
 * accessed and modified with Capsules(), AddCaspule(),
 * DeleteCapsule() and ClearCapsules().
 *
 * Visualization meta-data
 * =======================
 *
 * Basic visualization of Experiment data can be done through
 * `fort-studio`. Ants are visualized according to their DisplayStatus()
 * and DisplayColor(), which can be programmaticaly modified using
 * SetDisplayStatus() and SetDisplayColor().
 *
 * User defined meta-data (named values)
 * =====================================
 *
 * Ant can stores timed user defined metadata. These are modifiable
 * using SetValue() and DeleteValue() and accesible through
 * GetValue().
 *
 */
class Ant {
public:
	/**
	 * A pointer to an Ant
	 */
	typedef std::shared_ptr<Ant>       Ptr;

	/**
	 * The DisplayState of an Ant in an Experiment
	 */
	enum class DisplayState {
	                         /** Ant is visible */
	                         VISIBLE = 0,
	                         /** Ant is hidden */
	                         HIDDEN  = 1,
	                         /** Ant is visible and all non-soloed ant will be hidden. */
	                         SOLO    = 2,
	};


	/**
	 * Gets the ::TagID identifying this Ant at a given time.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.IdentifiedAt(time: py_fort_myrmidon.Time) -> int
	 * ```
	 * * R:
	 * ```R
	 * fmAntIdentifiedAt(ant,time = fmTimeForever() ) # returns an integer
	 * ```
	 * @param time the Time for which we want the identification
	 *
	 * Gets the ::TagID identifying this Ant at a given Time. If no
	 * Identification are valid for this time, an an exception will be
	 * thrown.
	 *
	 *
	 * @return a ::TagID that identify this ant at this time.
	 *
	 * @throws std::runtime_error if there no valid Identification for this time.
	 */
	TagID IdentifiedAt(const Time & time) const;


	/**
	 * Gets the Identification targetting this Ant.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.Identications() -> list(py_fort.myrmidon.Identification)
	 * ```
	 * * R:
	 * ```R
	 * fmAntIdentifications(ant) # returns a slist of Rcpp_fmIdentification
	 * ```
	 *
	 * Gets the Identification targetting this Ant. These
	 * Identification will always be sorted in Time and never overlaps.
	 *
	 * @return an Identification::List of Identification that target this object.
	 */
	Identification::List Identifications();

	// Gets the ID of an Ant
	//
	// Ants gets an unique ID in an experiment.
	//
	// R Version :
	// ```R
	// ant$antID()
	// ```
	//
	// @return the <ID> of the Ant
	fort::myrmidon::AntID AntID() const;

	// Gets the ID of the Ant formatted as a string.
	//
	// By Convention <ID> are formatted using hexadecimal notation (as
	// opposed to tag that are formatted decimal).
	//
	// R Version :
	// ```R
	// ant$formattedID()
	// ```
	//
	// @return a string with the formatted ID
	std::string FormattedID() const;

	// Gets the Display Color of an Ant
	//
	// Each Ant has a defined color for display.
	//
	// R Version :
	// ```R
	// ant$displayColor()
	// ```
	//
	// @return a const reference to the <Color> used to display the Ant
	const Color & DisplayColor() const;


	// Sets the Ant display color
	// @color the new <Color> to display the Ant
	//
	// R Version :
	// ```R
	// ant$setDisplayColor(color = fmRGBColor(r,g,b))
	// ```
	void SetDisplayColor(const Color & color);

	// Gets the Ant display state
	//
	// When interacting with the FORT Studio, any Ant has
	// different <DisplayState> :
	//
	//   * <DisplayState::VISIBLE>: the Ant is visible if
	//     they are no Ant which are <DisplayState::SOLO>
	//   * <DisplayState::HIDDEN>: the Ant is not displayed
	//   * <DisplayState::SOLO>: the Ant is visible and
	//     all non <DisplayState::SOLO> Ant are shown
	//
	// R Version :
	// ```R
	// s <- ant$displayStatus()
	// # to get the name of the value
	// names(which( s == fmAntDisplayState ) )
	// ```
	//
	// @return the <DisplayState> for this Ant.
	DisplayState DisplayStatus() const;

	// Sets the Ant display state.
	// @s the wanted <DisplayState>
	//
	// R Version :
	// ```R
	// ant$setDisplayStatus(fmAntDisplayState["SOLO"])
	// ```
	void SetDisplayStatus(DisplayState s);

	// Gets non-tracking data value
	// @name the name of the non-tracking data value
	// @time the <Time> we want the value for
	//
	// Gets the value for <name> at <time>. Values are set with
	// <SetValue>. If no value is sets prior to <time> (including -∞),
	// it will be using the <Experiment> default one.
	//
	// R Version :
	// ```R
	// ant$getValue(name,time)
	// ```
	//
	// @return the wanted <AntStaticValue>
	const AntStaticValue & GetValue(const std::string & name,
	                                const Time & time) const;

	// Sets a non-tracking data value at given Time
	// @name the wanted column name
	// @value the wanted <AntStaticValue>
	// @time the first <Time> after which <name> will be set to
	//       <value>. It can be <Time::SinceEver>.
	//
	// Sets <name> to <value> starting from <time>. If <time> is
	// nullptr, sets the starting <value> for <name>. <GetValue> is
	// always defined even if user does not define value for nullptr
	// <time> has the <Experiment> default value will be used.
	//
	// R Version :
	// ```R
	// time <- fmTimeParse("XXXX")$const_ptr() # could also be fmTimeInf()
	// ant$setValue(name,value,time)
	// ```
	void SetValue(const std::string & name,
	              const AntStaticValue & value,
	              const Time & time);

	// Removes any value defined for a time
	// @name the named value to remove
	// @time the <Time> to remove. It can be <Time::SinceEver>.
	//
	// Removes any value defined at a <Time>.
	//
	// R Version :
	// ```R
	// time <- fmTimeParse("XXXX")$const_ptr() # could also be fmTimeInf()
	// ant$deleteValue(name,time)
	// ```
	void DeleteValue(const std::string & name,
	                 const Time & time);


	// Adds a Capsule to the Ant virtual shape
	// @shapeTypeID the <AntShapeTypeID> for the <Capsule>
	// @capsule the <Capsule>
	//
	// Adds a <Capsule> to the Ant virtual shape, associated with the
	// <AntShapeTypeID> body part.
	//
	// R Version :
	// ```R
	// ant$addCapsule(shapeTypeID,fmCapsuleCreate(c(x1,y1),c(x2,y2),r1,r2))
	// ```
	void AddCapsule(AntShapeTypeID shapeTypeID,
	                const Capsule & capsule);

	// Gets all part of this ant
	//
	// R Version :
	// ```R
	// ant$capsulues()
	// ```
	//
	// @return a <TypedCapsuleList> representing the virtual shape of
	//         the Ant
	TypedCapsuleList Capsules() const;

	// Delete a part of the virtual shape
	// @index the index in the <Capsules> to remove
	//
	// R Version :
	// ```R
	// # ATTENTION, index are still starting from 0
	// ant$deleteCapsule(index)
	// ```
	void DeleteCapsule(const size_t index);

	// Deletes all virtual shape parts
	//
	// R Version :
	// ```R
	// ant$clearCapsules()
	// ```
	void ClearCapsules();

private:
	friend class Experiment;
	// Opaque pointer to implementation
	typedef const std::shared_ptr<priv::Ant> PPtr;

	// Private implementation constructor
	// @pAnt opaque pointer to implementation
	//
	// User cannot build Ant directly. They must be build and accessed
	// from <Experiment>.
	Ant(const PPtr & pAnt);

	Ant & operator=(const Ant &) = delete;
	Ant(const Ant &) = delete;

	PPtr d_p;
};


} // namespace myrmidon
} //namespace fort
