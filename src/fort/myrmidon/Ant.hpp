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



// The main object of interest of any Experiment
//
//
// Ant are the object of interest of an <Experiment>.
//
// ## Naming
//
// Ant are uniquely identified by <AntID>. By convention we use
// hexadecimal to display an <ID>, as returned by <FormattedID>.
//
// ## Identification
//
// Instead of working directly with <TagID> myrmidon uses
// <Identification> to relates <TagID> to an Ant. An Ant could have
// different <Identification>, allowing us to use different <TagID> to
// refer to the same individual. Myrmidon ensures the following
// invariant:
//
// * At any <Time> there is only a single <TagID> used to designate an
//   Ant
// * For any Ant, there only a single <Identification> that designate
//   this Ant at any given <Time>. I.e. <Identification::Start> and
//   <Identification::End> must not overlap for a given Ant.
//
// ## Visualization property
//
// Visualization of <Experiment> data is done through
// fort-studio. Ants are visualized according to their <DisplayStatus>
// and <DisplayColor>, which can be programmaticaly modified using
// <SetDisplayStatus> and <SetDisplayColor>. Ants are showed according
// to <DisplayState> value.
//
// ## Non-tracking data (named values)
//
// Ant also stores timed non-tracking data, called
// <named_values>. These are modifiable using <SetValue> and
// <DeleteValue> and accesible through <GetValue>.
//
// ## Ant Shapping
//
// Each Ant has a virtual shape that can be modified with <AddCapsule>
// and <DeleteCapsule>.
//
// More complete informations can be found in <ant_interaction>
class Ant {
public:
	// The ID of an Ant.
	//
	// ID are unique within an Experiment.
	typedef uint32_t                   ID;

	// The DisplayState of an Ant in an Experiment
	enum class DisplayState {
		// Ant is visible
		VISIBLE = 0,
		// Ant is hidden
		HIDDEN  = 1,
		// Ant is visible and all non-soloed ant will be hidden.
		SOLO    = 2
	};

	// Gets the const Identifications for this Ant
	//
	// Gets the <Identification> targetting this Ant. These
	// <Identification> will always be sorted in <Time> and not
	// overlapping.
	//
	// R Version :
	// ```R
	// ant$cIdentifications()
	// ```
	//
	// @return a  <Identification::ConstList>
	Identification::ConstList CIdentifications() const;

	// Gets the Identifications for this Ant
	//
	// Gets the <Identification> targetting this Ant. These
	// <Identification> will always be sorted in <Time> and not
	// overlapping.
	//
	// R Version :
	// ```R
	// ant$identifications()
	// ```
	//
	// @return an <Identification::List> copy of all
	//         <Identification>
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
	ID AntID() const;

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
	//
	// R Version :
	// ```R
	// ant$setDisplayColor(color = fmRGBColor(r,g,b))
	// ```
	//
	// @color the new <Color> for the Ant
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
	//       <value>
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
	              const Time::ConstPtr & time);

	// Removes any value defined for a time
	// @name the named value to remove
	// @time the <Time> to remove
	//
	// Removes any value defined at a <Time>.
	//
	// R Version :
	// ```R
	// time <- fmTimeParse("XXXX")$const_ptr() # could also be fmTimeInf()
	// ant$deleteValue(name,time)
	// ```
	void DeleteValue(const std::string & name,
	                 const Time::ConstPtr & time);


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


	// Opaque pointer to implementation
	typedef const std::shared_ptr<priv::Ant> PPtr;

	// Private implementation constructor
	// @pAnt opaque pointer to implementation
	//
	// User cannot build Ant directly. They must be build and accessed
	// from <Experiment>.
	Ant(const PPtr & pAnt);

private:
	PPtr d_p;
};


// const version of Ant
//
// Simply a strip down copy of <Ant> . Its an helper class
// to support const correctness of object and for language binding
// that does not enforce constness, such as R.
class CAnt {
public:
	// Gets the const Identifications for this Ant
	//
	// Gets the <Identification> targetting this Ant. These
	// <Identification> will always be sorted in <Time> and not
	// overlapping.
	//
	// R Version :
	// ```R
	// ant$cIdentifications()
	// ```
	//
	// @return a  <Identification::ConstList>
	Identification::ConstList CIdentifications() const;

	// Gets the ID of an Ant
	//
	// Ants gets an unique ID in an experiment.
	//
	// R Version :
	// ```R
	// ant$antID()
	// ```
	//
	// @return the <AntID> of the Ant
	fort::myrmidon::AntID AntID() const;

	// Gets the ID of the Ant formatted as a string.
	//
	// By Convention <AntID> are formatted using hexadecimal notation (as
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

	// Gets the Ant display state
	//
	// When interacting with the FORT Studio, any Ant has
	// different <Ant::DisplayState> :
	//
	//   * <Ant::DisplayState::VISIBLE>: the Ant is visible if
	//     they are no Ant which are <Ant::DisplayState::SOLO>
	//   * <Ant::DisplayState::HIDDEN>: the Ant is not displayed
	//   * <Ant::DisplayState::SOLO>: the Ant is visible and
	//     all non <Ant::DisplayState::SOLO> Ant are shown.
	//
	// R Version :
	// ```R
	// ant$displayStatus()
	// # to get the name of the value
	// names(which( s == fmAntDisplayState ) )
	// ```
	//
	// @return the <Ant::DisplayState> for this Ant.
	Ant::DisplayState DisplayStatus() const;

	// Gets non-tracking data value
	// @name the name of the non-tracking data value
	// @time the <Time> we want the value for
	//
	// Gets the value for <name> at <time>. Values are set with
	// <Ant::SetValue>. If no value is sets prior to <time> (including
	// -∞), it will be using the <Experiment> default one.
	//
	// R Version :
	// ```R
	// ant$getValue(name,time)
	// ```
	//
	// @return the wanted <AntStaticValue>
	const AntStaticValue & GetValue(const std::string & name,
	                                const Time & time) const;

	// Gets all part of this ant
	//
	// R Version :
	// ```R
	// ant$capsules()
	// ```
	//
	// @return a <TypedCapsuleList> representing the virtual shape of
	//         the Ant
	TypedCapsuleList Capsules() const;

	// Opaque pointer to implementation
	typedef const std::shared_ptr<const priv::Ant> ConstPPtr;

	// Private implementation constructor
	// @pAnt opaque pointer to implementation
	//
	// User cannot build Ant directly. They must be build and accessed
	// from <Experiment>.
	CAnt(const ConstPPtr & pAnt);

private:
	ConstPPtr d_p;

};


} // namespace myrmidon
} //namespace fort
