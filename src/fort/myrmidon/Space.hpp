#pragma once

#include <map>

#include "Zone.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Space;
} // namespace priv


/**
 *  An homogenous coordinate system for tracking data
 *
 * A Space represent the physical space tracked by one single
 * Camera. Coordinates in one Space cannot be compared with coordinate
 * from another Space.
 *
 * Space are uniquely identified with their SpaceID.
 *
 * Zoning
 * ======
 *
 * Within a single Space, it could be relevant to defines Zone where
 * interaction between Ant could be measured. I.e. Ant in different
 * Zone won't report interactions.
 *
 * Zone are manipulated with CreateZone() and DeleteZone().
 */
class Space {
public:
	/**
	 * Space indexed by their ID
	 */

	typedef std::map<SpaceID,Space> ByID;

	/**
	 * Gets the Space ID
	 *
	 * * Python: `py_fort_myrmidon.Space.ID (int)` read-only property of `py_fort_myrmidon.Space` objects
	 * * R:
	 * ```R
	 * fmSpaceID <- function(space) # returns an integer
	 * ```
	 *
	 * @return this Space SpaceID;
	 */
	SpaceID ID() const;

	/**
	 * Gets the Space name
	 *
	 * * Python: `py_fort_myrmidon.Space.Name (str)` read-write property of `py_fort_myrmidon.Space` objects
	 * * R:
	 * ```R
	 * fmSpaceName <- function(space) # returns a characther
	 * ```
	 *
	 * @return the Space name
	 */
	const std::string & Name() const;

	/**
	 * Sets the Space name
	 *
	 * * Python: `py_fort_myrmidon.Space.Name (str)` read-write property of `py_fort_myrmidon.Space` objects
	 * * R:
	 * ```R
	 * fmSpaceSetName <- function(space, name = '') # sets the space name
	 *
	 * @param name the wanted name
	 */
	void SetName(const std::string & name);

	/**
	 * Creates a new Zone in this Space
	 *
	 * * Python:
	 * ```
	 * py_fort_myrmidon.Space.CreateZone(self, name: str) -> py_fort_myrmidon.Zone
	 * ```
	 * * R :
	 * ```
	 * fmSpaceCreateZone <- function(space,name = '') # returns a Rcpp_fmZone
	 * ```
	 *
	 * @param name the Zone::Name()
	 *
	 * @return the newly created Zone
	 */
	Zone & CreateZone(const std::string & name);

	/**
	 * Deletes a Zone in this Space.
	 *
	 * * Python:
	 * ```
	 * py_fort_myrmidon.Space.DeleteZone(self, zoneID: int)
	 * ```
	 * * R :
	 * ```
	 * fmSpaceDeleteZone <- function(space,zoneID = 0)
	 * ```
	 *
	 * @param zoneID the ZoneID of the Zone to delete.
	 *
	 * @throws std::out_of_range if zoneID is not the ID of a Zone owned by this Space.
	 */
	void DeleteZone(ZoneID zoneID);

	/**
	 * Gets the Zones in this space
	 *
	 * Python: `py_fort_myrmidon.Zones (Dict[int,py_fort_myrmidon.Zone])` read-only property of `py_fort_myrmidon.Space` objects.
	 * R:
	 * ```R
	 * fmSpaceZones  <- function(space) # returns a named list of Rcpp_fmZone
	 * ```
	 * @return a map of Zone::ByID of all Zone in this Space.
	 */
	const Zone::ByID & Zones() const;

	/**
	 * Locates a movie file and frame number
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Space.LocateMovieFrame(self,time: py_fort_myrmidon.Time) -> Tuple[str,int]
	 * ```
	 * * R:
	 * ```R
	 * fmSpaceLocateMovieFrame <- function(space) # returns a list of a character and an integer
	 * ```
	 *
	 * @param time the Time we want a movie frame for.
	 *
	 * @return a pair of an absolute file path to the movie file, and
	 *         the wanted movie frame number.
	 *
	 * @throws std::out_of_range if a movie frame for the specified
	 * Time could not be found.
	 */
	std::pair<std::string,uint64_t> LocateMovieFrame(const Time & time) const;


	Space(Space &&) = default;
	Space & operator= (Space &&) = default;

private:
	friend class priv::Space;

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
