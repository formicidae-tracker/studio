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
 * Members
 * =======
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
	 *
	 * * Python: \todo descibe me
	 * * R:
	 * ```R
	 * fmAntDisplayState <- list(VISIBLE=0,HIDDEN=1,SOLO=2)
	 * ```
	 */
	enum class DisplayState {
	                         /**
	                          * the Ant is visible
	                          *
	                          * * Python: `py_fort_myrmidon.Ant.DisplayState.VISIBLE`
	                          * * R: `fmAntDisplayState$VISIBLE`
	                          */
	                         VISIBLE = 0,
	                         /**
	                          * the Ant is hidden
	                          *
 	                          * * Python: `py_fort_myrmidon.Ant.DisplayState.HIDDEN`
	                          * * R: `fmAntDisplayState$HIDDEN`
	                          */
	                         HIDDEN  = 1,
	                         /**
	                          * Ant is visible and all non-SOLO Ant will be hidden.
	                          *
	                          * * Python: `py_fort_myrmidon.Ant.DisplayState.SOLO`
	                          * * R: `fmAntDisplayState$SOLO`
	                          */
	                         SOLO    = 2,
	};

	/**
	 * Gets the ::TagID identifying this Ant at a given time.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.IdentifiedAt(self,time: py_fort_myrmidon.Time) -> int
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
	 * * Python: `Identificationn :list[py_fort.myrmidon.Identification]`
	 *   read-only property of `py_fort_myrmidon.Ant` objects
	 * * R:
	 * ```R
	 * fmAntIdentifications <- function(ant) # returns a slist of Rcpp_fmIdentification
	 * ```
	 *
	 * Gets the Identification targetting this Ant. These
	 * Identification will always be sorted in Time and never overlaps.
	 *
	 * @return an Identification::List of Identification that target this object.
	 */
	const Identification::List & Identifications();

	/**
	 *  Gets the AntID of an Ant.
	 *
	 * * Python: `py_fort_myrmidon.Ant.ID (int)` read-only property of `py_fort_myrmidon.Ant` objects
	 * * R:
	 * ```R
	 * fmAntGetID <- function(ant) # return an integer
	 * ```
	 *
	 * Ants gets an unique ID in an Experiment, with a minimal value
	 * of `1`. `0` is an invalid/undefined ::AntID.
	 *
	 * @return the ::AntID of the Ant
	 */
	fort::myrmidon::AntID ID() const;

	/**
	 * Gets the ID of the Ant formatted as a string.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.FormattedID(self) -> str
	 * ```
	 * * R:
	 * ```R
	 * fmAntFormattedID <- function(ant) # returns a character
	 * ```
	 *
	 * By Convention ::AntID are formatted using decimal notation with
	 * at least two `0` prefix (as opposed to ::TagID that are
	 * formatted using hexadecimal).
	 *
	 * @return a string with the formatted ID
	 */
	std::string FormattedID() const;

	/**
	 *  Gets the Display Color of an Ant.
	 *
	 * * Python: `DisplayColor: py_fort_myrmidon.Color` property of `py_fort_myrmidon.Ant` objects
	 * * R:
	 * ```R
	 * fmAntDisplayColor <- function(ant) # returns a vector of 3 integer
	 * ```
	 *
	 *  Each Ant has a defined Color for display in `fort-studio`.
	 *
	 * @return a const reference to the Color used to display the Ant
	 *         in `fort-studio`.
	 */
	const Color & DisplayColor() const;


	/**
	 * Sets the Ant display color
	 *
	 * * Python: `DisplayColor: py_fort_myrmidon.Color` property of `py_fort_myrmidon.Ant` objects
	 * * R:
	 * ```R
	 * fmAntSetDisplayColor <- function(ant,color = c(255,255,255))
	 * ```
	 *
	 * @param color the new Color to use to display the Ant in `fort-studio`.
	 */

	void SetDisplayColor(const Color & color);

	/**
	 *  Gets the Ant display state
	 *
	 * * Python: `DisplayStatus: py_fort_myrmidon.Ant.DisplayState` property of `py_fort_myrmidon.Ant` objects
	 * * R:
	 * ```R
	 * fmAntDisplayStatus <- function(ant) # returns an integer which is one of the value of in the fmAntDisplayState list
	 * ```
	 *
	 * When visualizing ant in `fort-studio`, any Ant has
	 * different DisplayState :
	 *
	 * * DisplayState::VISIBLE: the Ant is visible if
	 *   they are no other Ant which are in DisplayState::SOLO
	 * * DisplayState::HIDDEN: the Ant is not displayed
	 * * DisplayState::SOLO: the Ant is visible as any other Ant
	 *   which are in DisplayState::SOLO.
	 *
	 * @return the DisplayState for this Ant.
	 */
	DisplayState DisplayStatus() const;

	/**
	 * Sets the Ant display state.
	 *
	 * * Python: `DisplayStatus: py_fort_myrmidon.Ant.DisplayState` property of `py_fort_myrmidon.Ant` objects
	 * * R:
	 * ```R
	 * fmAntSetDisplayStatus <- function(ant,state = fmAntDisplayState$VISIBLE)
	 * ```
	 *
	 * @param s the wanted DisplayState
	 */
	void SetDisplayStatus(DisplayState s);

	/**
	 * Gets user defined timed metadata.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.GetValue(self,key :str,time :py_fort_myrmidon.Time) -> bool
	 * py_fort_myrmidon.Ant.GetValue(self,key :str,time :py_fort_myrmidon.Time) -> int
	 * py_fort_myrmidon.Ant.GetValue(self,key :str,time :py_fort_myrmidon.Time) -> float
	 * py_fort_myrmidon.Ant.GetValue(self,key :str,time :py_fort_myrmidon.Time) -> str
	 * py_fort_myrmidon.Ant.GetValue(self,key :str,time :py_fort_myrmidon.Time) -> py_fort_myrmidon.Time
	 * ```
	 * * R:
	 * ```R
	 * fmAntGetValue <- function(ant, name = '', time = fmTimeForever() ) # returns either a logical, integer, numerical, character or fmTime
	 * ```
	 *
	 * @param key the key of the user defined key in Experiment
	 * @param time the Time we want the value for (infinite Time are valid)
	 *
	 * Gets the value for a key at time. Values are set with
	 * SetValue(). If no value is sets prior to Time (including -∞),
	 * the Experiment default value for key will be returned.
	 *
	 * @return the wanted AntStaticValue for key at time, or the Experiment default one
	 *
	 * @throws std::out_of_range if name is not a defined metadata key in Experiment.
	 */
	const AntStaticValue & GetValue(const std::string & key,
	                                const Time & time) const;

	/**
	 *  Sets a user defined timed metadata
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.SetValue(self,key: str, value: , time: py_fort_myrmidon.Time)
	 * ```
	 * * R:
	 * ```R
	 * fmAntSetValue <- function(ant, name = '', value = false, time = fmTimeSinceEver())
	 * ```
	 *
	 * @param name the name of the user defined colum in Experiment
	 * @param value the desired AntStaticValue
	 * @param time the first Time after which name will be set to
	 *        value. It can be Time::SinceEver().
	 *
	 * Sets name to value starting from time. If time is
	 * Time::SinceEver(), sets the starting value for name instead of
	 * the Experiment's default value for name.
	 *
	 * @throws std::invalid_argument if name is not a defined key in Experiment
	 * @throws std::invalid_argument if time is Time::Forever()
	 * @throws std::invalid_argument if value is not of the right type for key
	 *
	 */
	void SetValue(const std::string & name,
	              const AntStaticValue & value,
	              const Time & time);

	/**
	 * Removes any user defined value at a given time
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.Delete(self,name: str, time: py_fort_myrmidon.Time)
	 * ```
	 * * R:
	 * ```R
	 * fmAntDeleteValue <- function(ant, name = '', time = fmTimeSinceEver())
	 * ```
	 *
	 * @param name the named value to remove
	 * @param time the Time to remove. It can be Time::SinceEver().
	 *
	 * Removes any value defined at a time.
	 *
	 * @throws std::out_of_range if no value for name at time have
	 *         been previously set with SetValue().
	 */
	void DeleteValue(const std::string & name,
	                 const Time & time);


	/**
	 *  Adds a Capsule to the Ant virtual shape list.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.AddCapsule(self,shapeTypeID: int, capsule: py_fort_myrmidon.Capsule)
	 * ```
	 * R:
	 * ```R
	 * fmAntAddCapsule <- function(ant, shapeTypeID = 0, capsule = fmCapsule() )
	 * ```
	 *
	 * @param shapeTypeID the AntShapeTypeID for the Capsule
	 * @param capsule the Capsule
	 *
	 * Adds a Capsule to the Ant virtual shape, associated with the
	 * shapeTypeID body part.
	 *
	 * @throws std::out_of_range if shapeTypeID is not defined in Experiment
	 */
	void AddCapsule(AntShapeTypeID shapeTypeID,
	                const std::shared_ptr<Capsule> & capsule);

	/**
	 * Gets all capsules for this Ant
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.Capsules(self) -> list((int,py_fort_myrmidon.Capsule))
	 * ```
	 * * R:
	 * ```R
	 * fmAntCapsules <- function(ant) # return a slist of Rcpp_fmCapsule
	 * ```
	 *
	 * @return a TypedCapsuleList representing the virtual shape of
	 *        the Ant
	 */
	const TypedCapsuleList & Capsules() const;

	/**
	 *  Delete a part of the virtual shape
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.DeleteCapsule(self, index: int)
	 * ```
	 * * R :
	 * ```R
	 * fmAntDeleteCapsule <- function(ant, index = 0) # WARNING index is a C index, starting at zero, not 1 !
	 * ```
	 * @param index the index in the Capsules() to remove
	 *
	 * @throws std::out_of_range if index is greate or equal to the size of Capsules().
	 */
	void DeleteCapsule(const size_t index);

	/**
	 *  Deletes all virtual shape parts
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Ant.DeleteCapsules(self)
	 * ```
	 * * R:
	 * ```R
	 * fmAntDeleteCapsules <- function(ant)
	 * ```
	 */
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
