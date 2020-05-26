#pragma once

#include <memory>

#include "Zone.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Space;
} // namespace priv


// const version of Space
//
// Simply a strip down copy of <Space> . Its an helper class
// to support const correctness of object and for language binding
// that does not enforce constness, such as R.
class CSpace {
public :
	// Gets the Space ID
	//
	// @return this Space <SpaceID>;
	fort::myrmidon::SpaceID SpaceID() const;

	// Gets the Space name
	//
	// Gets the Space name. Space names should be unique, valid
	// non-empty filename.
	//
	// @return the Space name
	const std::string & Name() const;

	// Gets the Zones in this space (const)
	//
	// @return a map of <Zone::ConstByID> of all <Zone> in this Space.
	Zone::ConstByID CZones() const;


	// Locates a movie file and frame number
	//
	// @return a pair of an absolute file path to the movie file, and
	// the wanted movie frame number.
	std::pair<std::string,uint64_t> LocateMovieFrame(const Time & time) const;

	// Opaque pointer for implementation
	typedef std::shared_ptr<const priv::Space> ConstPPtr;

	// Private implementation constructor
	// @pSpace opaque pointer to implementation
	//
	// User cannot build Space directly. They must be build and
	// accessed from <Experiment>.
	CSpace(const ConstPPtr & pSpace);
private:
	ConstPPtr d_p;
};

// An homogenous coordinate system for tracking data
//
// A Space represent the physical space tracked by one single
// Camera. Coordinates in one Space cannot be compared with coordinate
// from another Space.
//
// Space are uniquely identified with their <SpaceID>.
//
// ## Zoning
//
// Within a single Space, it could be relevant to defines <Zone> where
// interaction between Ant could be measured. I.e. <Ant> in different
// <Zone> won't report interactions.
//
// <Zone> are manipulated with <CreateZone> and <DeleteZone>.
class Space {
public:
	// A unique ID for a Space
	//
	// <SpaceID> are unique within an <Experiment>
	typedef uint32_t                     ID;

	// Gets the Space ID
	//
	// @return this Space <ID>;
	ID SpaceID() const;

	// Gets the Space name
	//
	// Gets the Space name. Space names should be unique, valid
	// non-empty filename.
	//
	// @return the Space name
	const std::string & Name() const;

	// Gets the Space name
	// @name the new Space name.
	//
	// Gets the Space name. Space names should be unique, valid
	// non-empty filename.
	void SetName(const std::string & name);

	// Creates a new Zone in this Space
	// @name the <Zone::Name>
	//
	// @return the new created <Zone>
	Zone CreateZone(const std::string & name);

	// Deletes a Zone in this Space.
	// @ID the <Zone::ID> to delete.
	void DeleteZone(Zone::ID ID);

	// Gets the Zones in this space
	//
	// @return a map of <Zone::ByID> of all <Zone> in this Space.
	Zone::ByID Zones();

	// Gets the Zones in this space (const)
	//
	// @return a map of <Zone::ConstByID> of all <Zone> in this Space.
	Zone::ConstByID CZones() const;


	// Locates a movie file and frame number
	//
	// @return a pair of an absolute file path to the movie file, and
	// the wanted movie frame number.
	std::pair<std::string,uint64_t> LocateMovieFrame(const Time & time) const;


	// Opaque pointer for implementation
	typedef std::shared_ptr<priv::Space> PPtr;

	// Private implementation constructor
	// @pSpace opaque pointer to implementation
	//
	// User cannot build Space directly. They must be build and
	// accessed from <Experiment>.
	Space(const PPtr & pSpace);
private:
	PPtr d_p;

};

} // namespace myrmidon
} // namespace fort
