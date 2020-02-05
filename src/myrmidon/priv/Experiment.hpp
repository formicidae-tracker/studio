#pragma once

#include <memory>

#include <fort-tags/fort-tags.h>

#include <myrmidon/Ant.hpp>
#include <myrmidon/Time.hpp>
#include <myrmidon/utils/FileSystem.hpp>

#include "FrameReference.hpp"

#include "ForwardDeclaration.hpp"
#include "LocatableTypes.hpp"

namespace fort {
namespace myrmidon {


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
	typedef std::map<fs::path,TrackingDataDirectoryConstPtr> TrackingDataDirectoryByURI;

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
	void AddTrackingDataDirectory(const TrackingDataDirectoryConstPtr & tdd);


	// Removes a TrackingDataDirectory
	//
	// @URI the URI of the directory
	void RemoveTrackingDataDirectory(const fs::path & URI);

	// Gets the TrackingDataDirectory related to this Experiment
	//
	// @return a map of all <priv::TrackingDataDirectory> related to this
	//         <priv::Experiment>, designated by their URI.
	const TrackingDataDirectoryByURI & TrackingDataDirectories() const;

	// Accessor to the underlying Identifier
	//
	// @return a reference to the underlying <Identifier>
	inline fort::myrmidon::priv::Identifier &  Identifier() {
		return *d_identifier;
	}

	// ConstAccessor to the underlying Identifier
	//
	// @return a reference to the underlying <Identifier>
	const fort::myrmidon::priv::Identifier & ConstIdentifier() const {
		return *d_identifier;
	}


	// The name of the Experiment.
	//
	// @return a reference to the experiment name
	const std::string & Name() const;
	// Sets the Experiment's name.
	//
	// @name the new <priv::Experiment> name
	void SetName(const std::string & name);

	// The author of the Experiment
	//
	// @return a reference to the author name
	const std::string & Author() const;
	// Sets the experiment's author
	//
	// @author the new value for the experiement author
	void SetAuthor(const std::string & author);

	// Comments about the experiment
	//
	// @return a reference to the <priv::Experiment> comment
	const std::string & Comment() const;
	// Sets the comment of the Experiment
	//
	// @comment the new experiment comment
	void SetComment(const std::string & comment);

	// The kind of tag used in the experiment
	//
	// @return the family of tag used in the experiment
	fort::tags::Family Family() const;
	// Sets the kind of tag used in the experiment
	//
	// @tf the tag that are used in the experiment
	void SetFamily(fort::tags::Family tf);

	// The default physical tag size
	//
	// Usually an Ant colony are tagged with a majority of tag of a
	// given size. This is this size. Some individuals (like Queens)
	// may often use a bigger tag size that should be set in their
	// Identification. This value is use for <Measurement>.
	//
	// @return the default tag size for the experiment in mm
	double DefaultTagSize() const;
	// Sets the default tag siye in mm
	//
	// @defaultTagSize the tag size in mm for the ma
	void   SetDefaultTagSize(double defaultTagSize);

	// The threshold used for tag detection
	//
	// @return the threshold used for detection
	uint8_t Threshold() const;

	// Sets the detection threshold
	//
	// @th the threshold to use.
	void SetThreshold(uint8_t th);

	// Adds or modifies a Measurement
	//
	// Adds a <Measurement> to the <priv::Experiment>.  Could also be
	// used to modifies an existing measurement.
	// @m the <Measurement> to add.
	void SetMeasurement(const MeasurementConstPtr & m);

	// Removes a Measurement
	//
	// @URI the URI of the measurement to remove
	void DeleteMeasurement(const fs::path & URI);

	void ListAllMeasurements(std::vector<MeasurementConstPtr> & list) const;

	struct ComputedMeasurement {
		Time   MTime;
		double LengthMM;
	};

	void ComputeMeasurementsForAnt(std::vector<ComputedMeasurement> & result,
	                               myrmidon::Ant::ID AID,
	                               MeasurementTypeID type) const;

	static double CornerWidthRatio(fort::tags::Family);

private:
	typedef std::map<uint32_t,MeasurementConstPtr>     MeasurementByType;
	typedef std::map<fs::path,MeasurementByType>       MeasurementByTagCloseUp;
	typedef std::map<uint32_t,
	                 std::map<TagID,
	                          std::map<fs::path,
	                                   std::map<Time,
	                                            MeasurementConstPtr,Time::Comparator>>>> SortedMeasurement;


	Experiment & operator=(const Experiment&) = delete;
	Experiment(const Experiment&)  = delete;

	bool ContainsFramePointer() const;

	Experiment(const fs::path & filepath);

	fs::path                    d_absoluteFilepath;
	fs::path                    d_basedir;
	TrackingDataDirectoryByPath d_dataDirs;
	IdentifierPtr               d_identifier;

	std::string        d_name;
	std::string        d_author;
	std::string        d_comment;
	fort::tags::Family d_family;
	double             d_defaultTagSize;
	uint8_t            d_threshold;

	MeasurementByTagCloseUp d_measurementByURI;
	SortedMeasurement       d_measurements;
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
