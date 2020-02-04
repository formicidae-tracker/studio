#pragma once

#include <memory>
#include <myrmidon/utils/FileSystem.hpp>

#include "../Ant.hpp"

#include "Identification.hpp"
#include "Shape.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

// The Object of Interest of any Experiment
//
// <priv::Ant> are the unique object of interest of an <Experiment> they are
// uniquely identified by their <myrmidon::Ant::ID>.
//
// <priv::Identification> relates an <priv::Ant> to a
// <myrmidon::TagID>. <Identifier::Identify> can be used to retrieve
// the <priv::Identification> associated to a <TagID> if it exist in any
// point in time of an <Experiment> defined by a <FramePointer>.
//
// An <priv::Ant> could have multiple <priv::Identification>
// associated with it, but they should not overlap in
// time. <Ant::SortAndCheckIdentifications> could be used to ensure
// this.
class Ant {
public:
	// A pointer to an Ant
	typedef std::shared_ptr<Ant> Ptr;

	// A List of shape
	typedef std::vector<Capsule::Ptr> Shapes;

	// The Constructor for an Ant
	Ant(fort::myrmidon::Ant::ID ID);

	// Its destructor
	~Ant();

	// The Identification::List associated with this priv::Ant.
	//
	// A sorted <Identification::List> associated with this
	// <priv::Ant>. Adding and removing of <Identification> have to be
	// respectively performed using <Identifier::AddIdentification>
	// and <Identifier::RemoveIdentification>.
	const Identification::List & Identifications() const;

	// Gets the Unique ID of the Ant
	//
	// <Ant> managed by the same <Identifier> are uniquely identified
	// by an Unique ID.
	//
	// @return the unique ID of the Ant
	fort::myrmidon::Ant::ID ID() const {
		return d_ID;
	}

	// Get a formatted ID.
	//
	// @return a string in the format "0xabcd"
	const std::string & FormattedID() const {
		return d_IDStr;
	}

	// Formats an Ant::ID to "0xabcd"
	//
	// @return the <myrmidon::Ant::ID> formatted in hexadecimal "0xabcd"
	static std::string FormatID(fort::myrmidon::Ant::ID ID);


	const Shapes & Shape() const;

	void AddCapsule(const Capsule::Ptr & capsule);

	// C++ shenanigans
	//
	// C++ shenanigan class to give restricted unlimited access to the
	// <Identification::List> to the <Identifier> class.
	class Accessor {
	private:
		static Identification::List & Identifications(Ant & a);
	public:
		friend class Identifier;
		friend class Identification;
	};

private:
	Ant & operator=(const Ant&) = delete;
	Ant(const Ant&)  = delete;


	fort::myrmidon::Ant::ID d_ID;
	std::string             d_IDStr;
	Identification::List    d_identifications;
	Shapes                  d_shape;
};

} //namespace priv

} // namespace myrmidon

} // namespace fort
