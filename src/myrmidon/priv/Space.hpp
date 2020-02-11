#pragma once

#include <map>

#include "LocatableTypes.hpp"
#include "ForwardDeclaration.hpp"

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
	// A pointer to a Space
	typedef std::shared_ptr<Space>       Ptr;
	// A cpnst pointer to a Space
	typedef std::shared_ptr<const Space> ConstPtr;

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

	// Exception sent when the desired Space is unknown
	class UnmanagedSpace : public std::runtime_error {
	public:
		// Constructor
		UnmanagedSpace(const fs::path & URI) noexcept;
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
		TDDAlreadyInUse(const fs::path & tddURI, const fs::path & spaceURI);
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

		typedef std::map<fs::path,TrackingDataDirectoryConstPtr> TrackingDataDirectoryByURI;

		static Space::Ptr Create(const Ptr & itself, const std::string & name);

		void DeleteSpace(const fs::path & URI);

		void DeleteTrackingDataDirectory(const fs::path & URI);

		const std::vector<Space::Ptr> & Spaces() const;

		const TrackingDataDirectoryByURI & TrackingDataDirectories() const;

	private:
		friend class Space;

		std::map<fs::path,Space::Ptr> d_spacesByURI;
		std::vector<Space::Ptr>       d_spaces;

		TrackingDataDirectoryByURI d_tddsByURI;
	};

	const fs::path & URI() const;

	void SetName(const std::string & name);

	void AddTrackingDataDirectory(const TrackingDataDirectoryConstPtr & tdd);

	const std::vector<TrackingDataDirectoryConstPtr> & TrackingDataDirectories() const;

private :
	Space(const std::string & name, const Universe::Ptr & universe);

	void DeleteTrackingDataDirectory(const fs::path & URI);


	Universe::Ptr LockUniverse() const;


	fs::path                d_URI;
	std::weak_ptr<Universe> d_universe;

	std::vector<TrackingDataDirectoryConstPtr> d_tdds;
};



} //namespace priv
} //namespace myrmidon
} //namespace fort
