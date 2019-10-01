#pragma once

#include <memory>

#include "Ant.hpp"
#include "TrackingDataDirectory.hpp"
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
	typedef std::unordered_map<std::string,TrackingDataDirectory> TrackingDataDirectoryByPath;
	typedef std::unordered_map<fort::myrmidon::Ant::ID,Ant::Ptr> AntByID;

	typedef std::unique_ptr<Experiment> Ptr;

	static Ptr Open(const std::filesystem::path & filename);
	static Ptr Create(const std::filesystem::path & filename);
	void Save(const std::filesystem::path & filename) const;

	std::filesystem::path AbsolutePath() const;

	void CheckDirectories();

	void AddTrackingDataDirectory(const TrackingDataDirectory & tdd);
	void RemoveTrackingDataDirectory(std::filesystem::path path);

	const TrackingDataDirectoryByPath & TrackingDataDirectories() const;

	Ant::Ptr CreateAnt();
	void DeleteAnt(fort::myrmidon::Ant::ID );
	const AntByID & Ants() const;

	const std::string & Name() const;
	void SetName(const std::string & name);

	const std::string & Author() const;
	void SetAuthor(const std::string & author);


	const std::string & Comment() const;
	void SetComment(const std::string & comment);

private:
	typedef std::set<fort::myrmidon::Ant::ID> SetOfID;

	Experiment(const std::filesystem::path & filepath);
	void Load(const std::filesystem::path & filepath);
	fort::myrmidon::Ant::ID NextAvailableID() const;





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
