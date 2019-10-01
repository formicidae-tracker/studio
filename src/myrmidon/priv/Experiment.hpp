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
	enum class TagFamily {Tag36h11=0,Tag36h10,Tag36ARTag,Tag16h5,Tag25h9,Circle21h7,Circle49h12,Custom48h12,Standard41h12,Standard52h13,Size};

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

	TagFamily Family() const;
	void SetFamily(TagFamily tf);

	uint8_t Threshold() const;
	void SetThreshold(uint8_t th);


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


inline std::ostream & operator<<( std::ostream & out,
                                  fort::myrmidon::priv::Experiment::TagFamily t) {
	static std::vector<std::string> names = {
		 "Tag36h11",
		 "Tag36h10",
		 "Tag36ARTag",
		 "Tag16h5",
		 "Tag25h9",
		 "Circle21h7",
		 "Circle49h12",
		 "Custom48h12",
		 "Standard41h12",
		 "Standard52h13",
		 "<unknown>",
	};
	size_t idx = (size_t)(t);
	if ( idx >= names.size() ) {
		idx = names.size()-1;
	}
	return out << names[idx];
}
