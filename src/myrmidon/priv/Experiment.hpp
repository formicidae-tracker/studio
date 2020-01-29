#pragma once

#include <memory>

#include <fort-tags/fort-tags.h>

#include "../Ant.hpp"
#include "../Time.hpp"
#include <myrmidon/utils/FileSystem.hpp>

#include "TrackingDataDirectory.hpp"
#include "Identifier.hpp"

#include "ForwardDeclaration.hpp"
#include "LocatableTypes.hpp"
namespace fort {
namespace myrmidon {

namespace pb {
class Experiment;
class TrackingDataDirectory;
}

namespace priv {


using namespace fort::myrmidon;

// Entry point of the myrmidon private interface
//
// An <priv::Experiment> is responsible to hold the links to the
// <TrackingDataDirectory> and to provide the <Identifier> for an
// actual Ant colony.
//
// It also provide the interface to read/write <priv::Experiment> to
// the filesystem.
//
// user may only to initialize a single object of this type, as
// <priv::Ant> will be only uniquely identified by theyr
// <myrmidon::Ant::ID> when they originate from the same
// <Identifier>. Anyway dataset are very large and we should not try
// to anlayse several of them in the same program.
class Experiment : public FileSystemLocatable {
public :
	// The AprilTag families supported by the FORT project.
	//
	// Maps <TrackingDataDirectory> by their path
	typedef std::unordered_map<std::string,TrackingDataDirectory::ConstPtr> TrackingDataDirectoryByPath;

	// A Pointer to an Experiment.
	typedef std::unique_ptr<Experiment> Ptr;

	// Opens an existing experiment given its fs::path
	// @filename the fs::path to the ".myrmidon" file
	// @return a <Ptr> to the <Experiment>.
	static Ptr Open(const fs::path & filename);

	// Creates a new <Experiment> given a fs::path
	// @filename the fs::path to the ".myrmidon" file
	//
	// Creates a new <Experiment> associated with the
	// <fs::path>. **This function does not create any file on
	// itself.** Use either <NewFile> or <Save>
	// @return a <Ptr> to the empty <Experiment>
	static Ptr Create(const fs::path & filename);

	// Creates a new <Experiment> on the filesystem.
	// @filename the fs::path to the ".myrmidon" file
	//
	// Creates a new experiment and save it on a new file.
	// TODO: Is it too much. mconsider removing it.
	// @return a <Ptr> to the empty <Experiment>
	static Ptr NewFile(const fs::path & filename);

	// Saves te Experiment to the filesystem
	// @filename the path to save too
	//
	//  Saves the <Experiment> to the filesystem. It consists only of
	//  lightweight metadata of each Ants, Identification, Measurement
	//  shapes. The actual tracking data is kept in the actual
	//  directory referred byt the TrackingDatadirectory.
	//
	// Saves the <priv::Experiment> data to the filesystem
	void Save(const fs::path & filename) const;


	// The absolute path of the Experiment
	// @return the absolute fs::path of the <priv::Experiment> on the
	//         filesysten
	const fs::path & AbsoluteFilePath() const override;

	// The parent dir of the Experiment
	//
	// Since <TrackingDataDirectory> are pointer to the actual data
	// directory on the filesystem, any Path is relative to this
	// <Basedir>
	// @return a fs::path to the base directory of this
	//         <priv::Experiment>.
	const fs::path & Basedir() const;


	// Adds a new TrackingDataDirectory
	// @tdd the new <TrackingDataDirectory> to add
	//
	// This methods adds the <TrackingDataDirectory> only if none of
	// its Frame overlaps in time with the <TrackingDataDirectory>
	// already referenced by this <priv::Experiment>.
	//
	// TODO: how to treat the case of multiple box experiment? In that
	// case the frame will overlap. But its the same colony. But we
	// have now two reference systems.
	void AddTrackingDataDirectory(const TrackingDataDirectory::ConstPtr & tdd);


	// Removes a TrackingDataDirectory
	// @path relative path to the directory
	void RemoveTrackingDataDirectory(fs::path path);

	// Gets the TrackingDataDirectory related to this Experiment
	// @return a map of all <TrackingDataDirectory> related to this
	//         <Experiment>.
	const TrackingDataDirectoryByPath & TrackingDataDirectories() const;

	// Accessor to the underlying Identifier
	// @return a reference to the underlying <Identifier>
	inline fort::myrmidon::priv::Identifier &  Identifier() {
		return *d_identifier;
	}

	// ConstAccessor to the underlying Identifier
	// @return a reference to the underlying <Identifier>
	const fort::myrmidon::priv::Identifier & ConstIdentifier() const {
		return *d_identifier;
	}

	const std::string & Name() const;
	void SetName(const std::string & name);

	const std::string & Author() const;
	void SetAuthor(const std::string & author);


	const std::string & Comment() const;
	void SetComment(const std::string & comment);

	fort::tags::Family Family() const;
	void SetFamily(fort::tags::Family tf);

	uint8_t Threshold() const;
	void SetThreshold(uint8_t th);


private:

	Experiment & operator=(const Experiment&) = delete;
	Experiment(const Experiment&)  = delete;

	bool ContainsFramePointer() const;

	Experiment(const fs::path & filepath);

	fs::path                    d_absoluteFilepath;
	fs::path                    d_basedir;
	TrackingDataDirectoryByPath d_dataDirs;
	Identifier::Ptr             d_identifier;

	std::string        d_name;
	std::string        d_author;
	std::string        d_comment;
	fort::tags::Family d_family;
	uint8_t            d_threshold;


};

} //namespace priv

} //namespace myrmidon

} //namespace fort

inline std::ostream & operator<<( std::ostream & out,
                                  fort::tags::Family t) {
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
