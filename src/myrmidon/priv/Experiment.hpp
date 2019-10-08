#pragma once

#include <memory>

#include "../Ant.hpp"
#include <filesystem>

#include "TrackingDataDirectory.hpp"
#include "Identifier.hpp"

namespace fort {
namespace myrmidon {

namespace pb {
class Experiment;
class TrackingDataDirectory;
}

namespace priv {

class Ant;
typedef std::shared_ptr<Ant> AntPtr;
typedef std::unordered_map<fort::myrmidon::Ant::ID,AntPtr> AntByID;

using namespace fort::myrmidon;



class Experiment {
public :
	enum class TagFamily {Tag36h11=0,Tag36h10,Tag36ARTag,Tag16h5,Tag25h9,Circle21h7,Circle49h12,Custom48h12,Standard41h12,Standard52h13,Unset};

	typedef std::unordered_map<std::string,TrackingDataDirectory> TrackingDataDirectoryByPath;

	typedef std::unique_ptr<Experiment> Ptr;

	static Ptr Open(const std::filesystem::path & filename);
	static Ptr Create(const std::filesystem::path & filename);
	static Ptr NewFile(const std::filesystem::path & filename);
	void Save(const std::filesystem::path & filename) const;

	const std::filesystem::path & AbsolutePath() const;
	const std::filesystem::path & Basedir() const;

	void AddTrackingDataDirectory(const TrackingDataDirectory & tdd);
	void RemoveTrackingDataDirectory(std::filesystem::path path);

	const TrackingDataDirectoryByPath & TrackingDataDirectories() const;

	inline fort::myrmidon::priv::Identifier &  Identifier() {
		return *d_identifier;
	}

	const fort::myrmidon::priv::Identifier & ConstIdentifier() const {
		return *d_identifier;
	}

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

	bool FreeRangeContaining(FramePointerPtr & start,
	                         FramePointerPtr & end,
	                         uint32_t tag, const FramePointer & f) const;

private:

	Experiment & operator=(const Experiment&) = delete;
	Experiment(const Experiment&)  = delete;

	bool ContainsFramePointer() const;

	Experiment(const std::filesystem::path & filepath);

	std::filesystem::path       d_absoluteFilepath;
	std::filesystem::path       d_basedir;
	TrackingDataDirectoryByPath d_dataDirs;
	Identifier::Ptr             d_identifier;

	std::string d_name;
	std::string d_author;
	std::string d_comment;
	TagFamily   d_family;
	uint8_t     d_threshold;


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
