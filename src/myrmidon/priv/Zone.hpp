#pragma once

#include <map>

#include "LocatableTypes.hpp"
#include "ForwardDeclaration.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class ZoneUTest;

// A Zone manages TrackingDataDirectory
//
// A Zone manages <TrackingDataDirectory>, in order to manages the
// invariant, that in a given Zone, no two <TrackingDataDirectory> are
// allowed to overlap in time. It also means that a given
// <TrackingDataDirectory> can only be assigned once in a Zone.
class Zone : public Identifiable {
public:
	// A pointer to a Zone
	typedef std::shared_ptr<Zone>       Ptr;
	// A cpnst pointer to a Zone
	typedef std::shared_ptr<const Zone> ConstPtr;

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
		UnmanagedTrackingDataDirectory(const fs::path & URI) noexcept;
	};

	// Exception sent when the desired Zone is unknown
	class UnmanagedZone : public std::runtime_error {
	public:
		// Constructor
		UnmanagedZone(const fs::path & URI) noexcept;
	};

	// Exception sent when the chosen name is invalid
	class InvalidName : public std::runtime_error {
	public:
		// Constructor
		InvalidName(const std::string & name,
		            const std::string & reason) noexcept;
	};

	// Exception sent when the Zone is not empty
	class ZoneNotEmpty : public std::runtime_error {
	public :
		ZoneNotEmpty(const Zone & z);

	private:
		static std::string BuildReason(const Zone & z);
	};

	// Exception sent when the TrackingDataDirectory is used in
	// another zone
	class TDDAlreadyInUse : public std::runtime_error {
	public:
		TDDAlreadyInUse(const fs::path & tddURI, const fs::path & zoneURI);
	};


	// Group manages several Zone to insure <priv::Experiment>
	// invariant.
	//
	// A Grouo manages several Zone all together to ensure the
	// following invariants:
	//   * No two <Zone> can have the same name
	//   * A given <TrackingDataDirectory> can only be assigned to a single zone
	//
	// The <Zone> are managing the invariant that no two
	// <TrackingDataDirectory> can overlap in time in this Zone.
	class Group {
	public:
		typedef std::shared_ptr<Group> Ptr;

		typedef std::map<fs::path,TrackingDataDirectoryConstPtr> TrackingDataDirectoryByURI;

		static Zone::Ptr Create(const Ptr & itself, const std::string & name);

		void DeleteZone(const fs::path & URI);

		void DeleteTrackingDataDirectory(const fs::path & URI);

		const std::vector<Zone::Ptr> & Zones() const;

		const TrackingDataDirectoryByURI & TrackingDataDirectories() const;

	private:
		friend class Zone;

		std::map<fs::path,Zone::Ptr> d_zonesByURI;
		std::vector<Zone::Ptr>       d_zones;

		TrackingDataDirectoryByURI d_tddsByURI;
	};

	const fs::path & URI() const;

	void SetName(const std::string & name);

	void AddTrackingDataDirectory(const TrackingDataDirectoryConstPtr & tdd);

	const std::vector<TrackingDataDirectoryConstPtr> & TrackingDataDirectories() const;

private :
	Zone(const std::string & name, const Group::Ptr & group);

	void DeleteTrackingDataDirectory(const fs::path & URI);


	Group::Ptr LockGroup() const;


	fs::path             d_URI;
	std::weak_ptr<Group> d_group;

	std::vector<TrackingDataDirectoryConstPtr> d_tdds;
};



} //namespace priv
} //namespace myrmidon
} //namespace fort
