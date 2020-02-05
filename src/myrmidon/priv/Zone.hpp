#pragma once

#include "LocatableTypes.hpp"
#include "ForwardDeclaration.hpp"

namespace fort {
namespace myrmidon {
namespace priv {


class Zone : public Identifiable {
public:
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

	Zone(const std::string & name);

	void AddTrackingDataDirectory(const TrackingDataDirectoryConstPtr & tdd);

	void DeleteTrackingDataDirectory(const fs::path & URI);

	const fs::path & URI() const;

	void SetName(const std::string & name);

	const std::vector<TrackingDataDirectoryConstPtr> & TrackingDataDirectories() const;


private :
	fs::path d_URI;
	std::vector<TrackingDataDirectoryConstPtr> d_tdds;
};



} //namespace priv
} //namespace myrmidon
} //namespace fort
