#pragma once

#include <map>

#include "LocatableTypes.hpp"
#include "ForwardDeclaration.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class ZoneUTest;

class Zone : public Identifiable {
public:
	typedef std::shared_ptr<Zone>       Ptr;
	typedef std::shared_ptr<const Zone> ConstPtr;

	class TDDOverlap : public std::runtime_error {
	public:
		TDDOverlap(const TrackingDataDirectoryConstPtr & a,
		           const TrackingDataDirectoryConstPtr & b) noexcept;

		const TrackingDataDirectoryConstPtr & A() const;
		const TrackingDataDirectoryConstPtr & B() const;
	private:
		static std::string BuildWhat(const TrackingDataDirectoryConstPtr & a,
		                             const TrackingDataDirectoryConstPtr & b) noexcept;
		TrackingDataDirectoryConstPtr d_a,d_b;
	};

	class UnmanagedTrackingDataDirectory : public std::runtime_error {
	public:
		UnmanagedTrackingDataDirectory(const fs::path & URI) noexcept;
	};

	class Manager {
	public:
		typedef std::shared_ptr<Manager> Ptr;

		typedef std::map<fs::path,TrackingDataDirectoryConstPtr> TrackingDataDirectoryByURI;
		Manager();

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
	Zone(const std::string & name, const Manager::Ptr & manager);

	void DeleteTrackingDataDirectory(const fs::path & URI);


	Manager::Ptr LockManager() const;


	fs::path               d_URI;
	std::weak_ptr<Manager> d_manager;

	std::vector<TrackingDataDirectoryConstPtr> d_tdds;
};



} //namespace priv
} //namespace myrmidon
} //namespace fort
