#pragma once

#include <map>

#include "LocatableTypes.hpp"
#include "ForwardDeclaration.hpp"
#include "Zone.hpp"
#include "ContiguousIDContainer.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class SpaceUTest;

// A Space manages TrackingDataDirectory
//
// A Space manages <TrackingDataDirectory>, in order to manages the
// invariant, that in a given Space, no two <TrackingDataDirectory> are
// allowed to overlap in time. It also means that a given
// <TrackingDataDirectory> can only be assigned once in a Space.
class Space : public Identifiable {
public:
	typedef uint32_t                     ID;
	// A pointer to a Space
	typedef std::shared_ptr<Space>       Ptr;
	// A cpnst pointer to a Space
	typedef std::shared_ptr<const Space> ConstPtr;

	typedef std::unordered_map<Zone::ID,Zone::Ptr> ZoneByID;

	// Exception sent when two TrackingDataDirectory overlaps in time.
	class TDDOverlap : public std::runtime_error {
	public:
		// Constructor from two TrackingDataDirectory
		TDDOverlap(const TrackingDataDirectoryConstPtr & a,
		           const TrackingDataDirectoryConstPtr & b) noexcept;
		// A Reference to the first TrackingDataDirectory
		//
		// @return a <TrackingDataDirectory::ConstPtr> to the first
		//         <TrackingDataDirectory>
		const TrackingDataDirectoryConstPtr & A() const;
		// A Reference to the second TrackingDataDirectory
		//
		// @return a <TrackingDataDirectory::ConstPtr> to the second
		//         <TrackingDataDirectory>
		const TrackingDataDirectoryConstPtr & B() const;
	private:
		static std::string BuildWhat(const TrackingDataDirectoryConstPtr & a,
		                             const TrackingDataDirectoryConstPtr & b) noexcept;
		TrackingDataDirectoryConstPtr d_a,d_b;
	};

	// Exception sent when the desired TrackingDataDirectory is unknown.
	class UnmanagedTrackingDataDirectory : public std::runtime_error {
	public:
		// Constructor
		UnmanagedTrackingDataDirectory(const std::string & URI) noexcept;
	};

	// Exception sent when the desired Space is unknown
	class UnmanagedSpace : public std::runtime_error {
	public:
		// Constructor
		UnmanagedSpace(const std::string & URI) noexcept;
	};

	// Exception sent when the chosen name is invalid
	class InvalidName : public std::runtime_error {
	public:
		// Constructor
		InvalidName(const std::string & name,
		            const std::string & reason) noexcept;
	};

	// Exception sent when the Space is not empty
	class SpaceNotEmpty : public std::runtime_error {
	public :
		SpaceNotEmpty(const Space & z);

	private:
		static std::string BuildReason(const Space & z);
	};

	// Exception sent when the TrackingDataDirectory is used in
	// another space
	class TDDAlreadyInUse : public std::runtime_error {
	public:
		TDDAlreadyInUse(const std::string & tddURI, const std::string & spaceURI);
	};


	// Universe manages several Space to insure <priv::Experiment>
	// invariant.
	//
	// A Grouo manages several Space all together to ensure the
	// following invariants:
	//   * No two <Space> can have the same name
	//   * A given <TrackingDataDirectory> can only be assigned to a single space
	//
	// The <Space> are managing the invariant that no two
	// <TrackingDataDirectory> can overlap in time in this Space.
	class Universe {
	public:
		typedef std::shared_ptr<Universe> Ptr;

		typedef std::map<std::string,TrackingDataDirectoryConstPtr> TrackingDataDirectoryByURI;

		const static Space::ID NEXT_AVAILABLE_ID = 0;

		static Space::Ptr Create(const Ptr & itself,
		                         Space::ID spaceID,
		                         const std::string & name);

		void DeleteSpace(Space::ID spaceID);

		void DeleteTrackingDataDirectory(const std::string & URI);

		const SpaceByID & Spaces() const;

		const TrackingDataDirectoryByURI & TrackingDataDirectories() const;

		std::pair<Space::Ptr,TrackingDataDirectoryConstPtr>
		LocateTrackingDataDirectory(const std::string & tddURI) const;

		Space::Ptr LocateSpace(const std::string & spaceName) const;



	private:
		friend class Space;

		ContiguousIDContainer<Space::Ptr,Space::ID> d_spaces;

		TrackingDataDirectoryByURI d_tddsByURI;
	};

	const std::string & URI() const override;

	const std::string & Name() const;

	void SetName(const std::string & name);

	void AddTrackingDataDirectory(const TrackingDataDirectoryConstPtr & tdd);

	const std::vector<TrackingDataDirectoryConstPtr> & TrackingDataDirectories() const;

	const static Zone::ID NEXT_AVAILABLE_ID = 0;

	Zone::Ptr CreateZone(Zone::ID ID = NEXT_AVAILABLE_ID);

	void DeleteZone(Zone::ID ID);

	const ZoneByID & Zones() const;

	Space::ID SpaceID() const;

private :
	typedef std::set<Zone::ID> SetOfZoneID;
	Space(ID spaceID, const std::string & name, const Universe::Ptr & universe);

	void DeleteTrackingDataDirectory(const std::string & URI);


	Universe::Ptr LockUniverse() const;

	ID                      d_ID;
	std::string             d_URI;
	std::string             d_name;
	std::weak_ptr<Universe> d_universe;

	std::vector<TrackingDataDirectoryConstPtr> d_tdds;

	ContiguousIDContainer<Zone::Ptr,Zone::ID> d_zones;
};



} //namespace priv
} //namespace myrmidon
} //namespace fort
