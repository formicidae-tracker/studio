#pragma once

#include <memory>

#include "Color.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
class Ant;
} // namespace priv


// The main object of interest of any Experiment
//
// Ant are the object of interest of an Experiment. They are uniquely
// identified by <Ant::AntID>. By convention we use hexadecimal to
// display an <Ant::ID>, as returned by <Ant::FormattedID>
class Ant {
public:
	// A pointer to a an Ant
	typedef std::shared_ptr<Ant>       Ptr;
	// A const pointer to an Ant
	typedef std::shared_ptr<const Ant> ConstPtr;
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

	//const Identification::ConstList & CIdentifications() const
	//const Identification::ConstList & CIdentifications() const

	// Gets the ID of tha Ant
	//
	// <Ant> gets an unique ID in an experiment.
	// @return the <ID> of the <Ant>
	ID AntID() const;

	// Gets the ID of the Ant formatted as a string.
	//
	// By Convention <ID> are formatted using hexadecimal notation (as
	// opposed to tag that are formatted decimal).
	// @return a string with the FormattedID
	std::string FormattedID() const;

	// Gets the Display Color of an Ant
	//
	// Each <Ant> has a defined color for display.
	// @return a const reference to the <Color> used to display the Ant
	const Color & DisplayColor() const;











private:
	typedef const std::shared_ptr<priv::Ant> PPtr;
	Ant(const PPtr & pAnt);


	PPtr d_p;






};


} // namespace myrmidon
} //namespace fort
