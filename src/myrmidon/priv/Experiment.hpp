#pragma once

#include <memory>

#include "Ant.hpp"

#include <filesystem>
#include <chrono>

#include <google/protobuf/timestamp.pb.h>

namespace fort {
namespace myrmidon {

namespace pb {
class Experiment;
class TrackingDataDirectory;
}

namespace priv {

using namespace fort::myrmidon;

class Experiment {
public :
	struct TrackingDataDirectory {
		TrackingDataDirectory();
		TrackingDataDirectory(const pb::TrackingDataDirectory & tdd);
		std::filesystem::path  Path;

		uint64_t StartFrame;
		uint64_t EndFrame;

		google::protobuf::Timestamp StartDate,EndDate;
	};
	typedef std::unordered_map<std::string,TrackingDataDirectory> TrackingDataDirectoryByPath;
	typedef std::unordered_map<fort::myrmidon::Ant::ID,Ant::Ptr> AntByID;

	typedef std::unique_ptr<Experiment> Ptr;

	static Ptr Open(const std::filesystem::path & filename);
	void Save(const std::filesystem::path & filename) const;

	void CheckDirectories();

	void AddTrackingDataDirectory(const std::filesystem::path & path);
	void RemoveTrackingDataDirectory(std::filesystem::path path);

	const TrackingDataDirectoryByPath & TrackingDataPaths() const;

	Ant::Ptr CreateAnt();
	void DeleteAnt(fort::myrmidon::Ant::ID );
	const AntByID & Ants() const;

private:
	typedef std::set<fort::myrmidon::Ant::ID> SetOfID;

	Experiment(const std::filesystem::path & filepath);
	void Load(const std::filesystem::path & filepath);
	fort::myrmidon::Ant::ID NextAvailableID() const;

	void LoadFromFSTrackingDataDirectory(const std::filesystem::path & path,
	                                     TrackingDataDirectory & tdd);




	pb::Experiment              d_experiment;

	std::filesystem::path       d_absoluteFilepath;
	TrackingDataDirectoryByPath d_dataDirs;
	AntByID                     d_ants;
	SetOfID                     d_antIDs;
	bool                        d_continuous;
};

} //namespace priv

} //namespace myrmidon

} //namespace fort
