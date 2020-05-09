#pragma once

#include <memory>

#include "Zone.hpp"

namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> implementation
class Space;
} // namespace priv

class Space {
public:
	typedef std::shared_ptr<Space>       Ptr;
	typedef std::shared_ptr<const Space> ConstPtr;
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
	void SetName(const std::string & name) const;

	// Creates a new Zone in this Space
	// @name the <Zone::Name>
	//
	// @return a <Zone::Ptr> for the new created Zone
	Zone::Ptr CreateZone(const std::string & name);

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


	// Opaque pointer for implementation
	typedef std::shared_ptr<priv::Space> PPtr;

	// Private implementation constructor
	// @pptr opaque pointer to implementation
	//
	// User cannot build Space directly. They must be build and
	// accessed from <Experiment>.
	Space(const PPtr & pSpace);
private:
	PPtr d_p;

};

} // namespace myrmidon
} // namespace fort
