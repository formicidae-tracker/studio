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

	// Exception sent when two TrackingDataDirectory overlaps in time.
	class TDDOverlap : public std::runtime_error {
	public:
		// Constructor from two TrackingDataDirectory
		TDDOverlap(const TrackingDataDirectoryPtr & a,
		           const TrackingDataDirectoryPtr & b) noexcept;
		// A Reference to the first TrackingDataDirectory
		//
		// @return a <TrackingDataDirectory::ConstPtr> to the first
		//         <TrackingDataDirectory>
		const TrackingDataDirectoryPtr & A() const;
		// A Reference to the second TrackingDataDirectory
		//
		// @return a <TrackingDataDirectory::ConstPtr> to the second
		//         <TrackingDataDirectory>
		const TrackingDataDirectoryPtr & B() const;
	private:
		static std::string BuildWhat(const TrackingDataDirectoryPtr & a,
		                             const TrackingDataDirectoryPtr & b) noexcept;
		TrackingDataDirectoryPtr d_a,d_b;
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

		typedef std::map<std::string,TrackingDataDirectoryPtr> TrackingDataDirectoryByURI;

		const static Space::ID NEXT_AVAILABLE_SPACE_ID = 0;

		static Space::Ptr Create(const Ptr & itself,
		                         Space::ID spaceID,
		                         const std::string & name);

		void DeleteSpace(Space::ID spaceID);

		void DeleteTrackingDataDirectory(const std::string & URI);

		const ConstSpaceByID & CSpaces() const;
		const SpaceByID & Spaces();

		const TrackingDataDirectoryByURI & TrackingDataDirectories() const;

		std::pair<Space::ConstPtr,TrackingDataDirectoryPtr>
		CLocateTrackingDataDirectory(const std::string & tddURI) const;

		std::pair<Space::Ptr,TrackingDataDirectoryPtr>
		LocateTrackingDataDirectory(const std::string & tddURI);


		Space::ConstPtr CLocateSpace(const std::string & spaceName) const;

		Space::Ptr LocateSpace(const std::string & spaceName);



	private:
		friend class Space;

		AlmostContiguousIDContainer<Space::ID,Space> d_spaces;

		TrackingDataDirectoryByURI d_tddsByURI;
	};

	virtual ~Space();

	const std::string & URI() const override;

	const std::string & Name() const;

	void SetName(const std::string & name);

	const std::vector<TrackingDataDirectoryPtr> & TrackingDataDirectories() const;

	const static Zone::ID NEXT_AVAILABLE_ID = 0;

	Zone::Ptr CreateZone(const std::string & name, Zone::ID ID = NEXT_AVAILABLE_ID);

	void DeleteZone(Zone::ID ID);

	const ConstZoneByID & CZones() const;
	const ZoneByID & Zones();

	Space::ID SpaceID() const;

	class Accessor {
		static void AddTrackingDataDirectory(const Space::Ptr & itself,
		                                     const TrackingDataDirectoryPtr & tdd);

		friend class fort::myrmidon::priv::Experiment;
	};

private :
	friend class SpaceUTest_CanHoldTDD_Test;
	friend class SpaceUTest_ExceptionFormatting_Test;

	typedef std::set<Zone::ID> SetOfZoneID;
	Space(ID spaceID, const std::string & name, const Universe::Ptr & universe);


	void AddTrackingDataDirectory(const TrackingDataDirectoryPtr & tdd);

	void DeleteTrackingDataDirectory(const std::string & URI);


	Universe::Ptr LockUniverse() const;

	ID                      d_ID;
	std::string             d_URI;
	std::string             d_name;
	std::weak_ptr<Universe> d_universe;

	std::vector<TrackingDataDirectoryPtr> d_tdds;

	AlmostContiguousIDContainer<Zone::ID,Zone> d_zones;
};



} //namespace priv
} //namespace myrmidon
} //namespace fort
