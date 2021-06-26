#pragma once

#include <memory>

#include "Zone.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Space;
} // namespace priv


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
	// A pointer to a Space
	typedef std::shared_ptr<Space>       Ptr;

	// Gets the Space ID
	//
	// R version:
	// ```R
	// s$spaceID()
	// ```
	//
	// @return this Space <ID>;
	SpaceID ID() const;

	// Gets the Space name
	//
	// Gets the Space name. Space names should be unique, valid
	// non-empty filename.
	//
	// R version:
	// ```R
	// s$name()
	// ```
	//
	// @return the Space name
	const std::string & Name() const;

	// Gets the Space name
	// @name the new Space name.
	//
	// Gets the Space name. Space names should be unique, valid
	// non-empty filename.
	//
	// R version:
	// ```R
	// s$setName(name)
	// ```
	void SetName(const std::string & name);

	// Creates a new Zone in this Space
	// @name the <Zone::Name>
	//
	// R version:
	// ```R
	// zoneID <- s$createZone(name)
	// ```
	//
	// @return the new created <Zone>
	Zone::Ptr CreateZone(const std::string & name);

	// Deletes a Zone in this Space.
	// @ID the <Zone::ID> to delete.
	//
	// R version:
	// ```R
	// s$deleteZone(zoneID)
	// ```
	void DeleteZone(ZoneID ID);

	// Gets the Zones in this space
	//
	// R version:
	// ```R
	// s$zones()
	// ```
	//
	// @return a map of <Zone::ByID> of all <Zone> in this Space.
	Zone::ByID Zones();

	// Locates a movie file and frame number
	// @time the <Time> we want a movie for.
	//
	// R version:
	// ```R
	// s$locateMovieFrame(fmTimeParse("XXX"))
	// ```
	//
	// @return a pair of an absolute file path to the movie file, and
	//         the wanted movie frame number.
	std::pair<std::string,uint64_t> LocateMovieFrame(const Time & time) const;


private:
	friend class Experiment;

	// Opaque pointer for implementation
	typedef std::shared_ptr<priv::Space> PPtr;

	// Private implementation constructor
	// @pSpace opaque pointer to implementation
	//
	// User cannot build Space directly. They must be build and
	// accessed from <Experiment>.
	Space(const PPtr & pSpace);

	Space & operator=(const Space &) = delete;
	Space(const Space &) = delete;

	PPtr d_p;
};

} // namespace myrmidon
} // namespace fort
