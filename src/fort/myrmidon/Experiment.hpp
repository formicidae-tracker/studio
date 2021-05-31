#pragma once

#include <memory>

#include <fort/tags/fort-tags.hpp>

#include "Types.hpp"
#include "Ant.hpp"
#include "Space.hpp"
#include "TrackingSolver.hpp"


namespace fort {
namespace myrmidon {

namespace priv {
// private <fort::myrmidon::priv> Implementation
class Experiment;
// private <fort::myrmidon::priv> Implementation
class TrackingSolver;
} // namespace priv


class Query;


// Entry point of myrmidon API
//
// Experiment is the main entry point of the myrmidon API. An
// Experiment olds a collection of <Ant>, <Identification>, <Space>
// and <Zone> measurement and give access to the identified tracking
// data and basic interaction detection.
//
// ## File convention
//
// Experiment are save to the disk in `.myrmidon` files. One can use
// <Open>, <OpenReadOnly>, <Save> and <NewFile> to interact with those
// files.
//
// Experiment saves relative links to the tracking data directory that
// are acquired with the FORT system. These path are relative, so one
// can rename a `.myrmidon` file on the filesystem, but it must keep
// the same relative path to these tracking data directories.
//
// Multiple program can open an Experiment in read-only mode. However
// a single program can at a Time open an Experiment with full access
// ( read-only program must have released the Experiment too !!!).
class Experiment {
public:
	// Opens an Experiment with full access
	// @filepath the path to the wanted file
	//
	// Opens an Experiment with full access to a `.myrmidon`
	// file. Only a single program can open the same myrmidon file
	// with full access (read only access must be closed).
	//
	// R Version :
	// ```R
	// fmExperimentOpen(path)
	// ```
	//
	// @return the <Experiment>
	static Experiment Open(const std::string & filepath);

	// Opens an Experiment without associated tracking data
	// @filepath the path to the wanted file.
	//
	// Opens an Experiment to a `.myrmidon` file without opening its
	// associated tracking data. Please also note that the file will
	// be opened in Read Only mode. This functionalities is useful to
	// be able to parse the Ant desciption list, and for example
	// identify or collides ants from realtime tracking data acquired
	// over the network. When opened in 'data-less' mode, no tracking
	// data, tag statistic or measurement will be returned ( the
	// experiment will appear empty ). However
	// <TrackingSolver::IdentifyFrame> and
	// <TrackingSolver::CollideFrame> will work as expected as the
	// user is required to provide the data to these function.
	//
	//
	// R Version :
	// ```R
	// fmExperimentOpenDataLess(path)
	// ```
	//
	// @return the <Experiment>
	static const Experiment OpenDataLess(const std::string & filepath);

	// Creates a new Experiment file
	// @filepath the wanted filepath
	//
	// Creates a new Experiment at the wanted filesystem
	// location. Will throw an error if a file already exists at this
	// location.
	//
	// R Version :
	// ```R
	// fmExperimentNewFile(path)
	// ```
	//
	// @return  the new empty <Experiment>
	static Experiment NewFile(const std::string & filepath);

	// Creates a new Experiment without file association
	// @filepath the wanted filepath
	//
	// Creates a new Experiment virtually associated with the desired
	// filesystem location. Will not create a file.
	//
	// R Version :
	// ```R
	// fmExperimentCreate(path)
	// ```
	//
	// @return the new empty <Experiment>
	static Experiment Create(const std::string & filepath);

	// Saves the Experiment
	// @filepath the desired filesystem location to save the Experiment to
	//
	// Saves the Experiment to its location. It is forbidden to change
	// its parent directory (but file renaming is permitted).
	//
	// R Version :
	// ```R
	// e$save(path)
	// ```
	void Save(const std::string & filepath);


	// Path to the underlying `.myrmidon` file
	//
	// R Version :
	// ```R
	// e$absoluteFilePath()
	// ```
	//
	// @return the path to the `.myrmidon` file
	std::string AbsoluteFilePath() const;

	// Creates a new <Space>
	// @name wanted name for the new <Space>
	//
	// R Version :
	// ```R
	// e$createSpace(name)
	// ```
	//
	// @return the new <Space>
	Space CreateSpace(const std::string & name);

	// Deletes a <Space>
	// @spaceID the <Space::ID> of the <Space> we want to delete.
	//
	// R Version :
	// ```R
	// e$deleteSpace(spaceID)
	// ```
	void DeleteSpace(Space::ID spaceID);

	// Gets the <Space> in the Experiment
	//
	// R Version :
	// ```R
	// e$spaces()
	// ```
	//
	// @return a map of the Experiment <Space> by their <Space::ID>
	std::map<Space::ID,Space::Ptr> Spaces();

	// Gets the <Space> in the Experiment with const access
	//
	// R Version :
	// ```R
	// e$cSpaces()
	// ```
	//
	// @return a const map of the Experiment <Space>
	std::map<Space::ID,Space::ConstPtr> CSpaces() const;

	// Gets the <Space> in the Experiment (const overload)
	//
	// R Version :
	// ```R
	// e$cSpaces()
	// ```
	//
	// @return a const map of the Experiment <Space>
	std::map<Space::ID,Space::ConstPtr> Spaces() const;


	// Adds a tracking data directory to Experiment
	// @spaceID the <Space> the data directory is associated with
	// @filepath path to the directory we want to add
	//
	// Adds a tracking data director acquired with the fort system to
	// the wanted <Space>.
	//
	// R Version :
	// ```R
	// e$addTrackingDataDirectory(spaceID,filepath)
	// ```
	// @return the URI used to designate the tdd
	std::string AddTrackingDataDirectory(Space::ID spaceID,
	                                     const std::string & filepath);

	// Removes a Tracking Data Directory from the Experiment
	// @URI the URI of the tracking data directory to remove
	//
	// R Version :
	// ```R
	// e$deleteTrackingDataDirectory(uri)
	// ```
	void DeleteTrackingDataDirectory(const std::string & URI);

	// Creates a new <Ant>
	//
	// R Version :
	// ```R
	// ant <- e$createAnt()
	// ```
	//
	// @return the new <Ant>
	Ant::Ptr CreateAnt();

	// Gets the <Ant> in the Experiment
	//
	// R Version :
	// ```R
	// e$ants()
	// ```
	//
	// @return the <Ant> indexed by their <Ant::ID> in the Experiment.
	std::map<Ant::ID,Ant::Ptr> Ants();

	// Gets the <Ant> in the Experiment
	//
	// R Version :
	// ```R
	// e$cAnts()
	// ```
	//
	// @return the const <Ant> indexed by their <Ant::ID> in the
	// Experiment.
	std::map<Ant::ID,Ant::ConstPtr> CAnts() const;

	// Gets the <Ant> in the Experiment (const overload)
	//
	// R Version :
	// ```R
	// e$cAnts()
	// ```
	//
	// @return the const <Ant> indexed by their <Ant::ID> in the
	// Experiment.
	std::map<Ant::ID,Ant::ConstPtr> Ants() const;

	// Adds an <Identification> to the Experiment
	// @antID the targetted <Ant> designated by its <Ant::ID>
	// @tagID the tag to associate with the Ant
	// @start the first valid <Time>. It can be <Time::SinceEver>
	// @end  the first invalid <Time>. It can be <Time::Forever>
	//
	// Adds an <Identification> to the Experiment. <Identification>
	// are valid for [<start>,<end>[. One may obtain a valid time
	// range using <FreeIdentificationRangeAt>.
	//
	// R Version :
	// ```R
	// start <- fmTimeParse("XXX")$const_ptr() # or fmTimeInf()
	// end <- fmTimeParse("XXX")$const_ptr() # or fmTimeInf()
	// e$addIdentification(antID,tagID,start,end)
	// ```
	//
	// @return the new <Identification>
	Identification::Ptr AddIdentification(Ant::ID antID,
	                                      TagID tagID,
	                                      const Time & start,
	                                      const Time & end);

	// Deletes an <Identification>
	// @identification the <Identification> to delete
	//
	// R Version :
	// ```R
	// # i is the wanted fmIdentification to delete
	// e$deleteIdentification(i)
	// ```
	void DeleteIdentification(const Identification::ConstPtr & identification);

	// Queries for a valid time range
	// @start return value by reference for the start of the range
	// @end return value by reference for the end of the range
	// @tagID the <TagID> we want a range for
	// @time the <Time> that must be included in the result time range
	//
	// Queries for a valid time range for a given <TagID> and
	// <Time>. The result will be a range [<start>,<end>[ containing
	// <time> where <tagID> is not used. It returns false if such a
	// range does not exists. <start> and <end> can respectively be
	// set to <Time::SinceEver> and <Time::Forever>.
	//
	// R Version :
	// ```R
	// # initializes return value as fmTimeCPtr
	// start <- new ( FortMyrmidon::fmTimeCPtr )
	// end <- new ( FortMyrmidon::fmTimeCPtr )
	// e$freeIdentificationRangeAt(start,end,tagID,time)
	// ```
	//
	// @return `true` if such a range exist, false otherwise.
	bool FreeIdentificationRangeAt(Time & start,
	                               Time & end,
	                               TagID tagID,
	                               const Time & time) const;

	// The name of the Experiment.
   	//
	// R Version :
	// ```R
	// e$name()
	// ```
	//
	// @return a reference to the Experiment's name
	const std::string & Name() const;

	// Sets the Experiment's name.
	// @name the new <priv::Experiment> name
	//
	// R Version :
	// ```R
	// e$setName(name)
	// ```
	void SetName(const std::string & name);

	// The author of the Experiment
	//
	// R Version :
	// ```R
	// e$author()
	// ```
	//
	// @return a reference to the author name
	const std::string & Author() const;

	// Sets the Experiment's author
	// @author the new value for the Experiement's author
	//
	// R Version :
	// ```R
	// e$setAuthor(author)
	// ```
	void SetAuthor(const std::string & author);

	// Comments about the experiment
	//
	// R Version :
	// ```R
	// e$comment()
	// ```
	//
	// @return a reference to the Experiment's comment
	const std::string & Comment() const;

	// Sets the comment of the Experiment
	// @comment the wnated Experiment's comment
	//
	// R Version :
	// ```R
	// e$setComment(comment)
	// ```
	void SetComment(const std::string & comment);

	// The kind of tag used in the Experiment
	//
	// R Version :
	// ```R
	// f <- e$family()
	// names(which( f == fmTagFamily ) )
	// ```
	//
	// @return the family of tag used in the Experiment
	fort::tags::Family Family() const;

	// The default physical tag size
	//
	// Usually an Ant colony are tagged with a majority of tag of a
	// given size. Some individuals (like Queens) may often use a
	// bigger tag size that should be set in their
	// Identification. This value is used for
	// <Query::ComputeMeasurementFor>.
	//
	// myrmidon uses without white border convention for ARTag and
	// with white border convention Apriltag.
	//
	// R Version :
	// ```R
	// e$defaultTagSize()
	// ```
	//
	// @return the default tag size for the experiment in mm
	double DefaultTagSize() const;

	// Sets the default tag siye in mm
	// @defaultTagSize the tag size in mm ( the one defined on tag sheet )
	//
	// R Version :
	// ```R
	// # sets the tag size to 0.7 mm
	// e$setDefaultTagSize(0.7)
	// ```
	void   SetDefaultTagSize(double defaultTagSize);


	/* cldoc:begin-category(manual_measurement) */

	// Creates a measurement type
	// @name the wanted name for the new measurement
	//
	// R Version :
	// ```R
	// e$createMeasurementType("foo")
	// ```
	//
	// @return the <MeasurementTypeID> identifying the new measurement
	//         type
	MeasurementTypeID CreateMeasurementType(const std::string & name);

	// Deletes a measurement type
	// @mTypeID the <MeasurementTypeID> to delete
	//
	// R Version :
	// ```R
	// e$deleteMeasurementType(mTypeID)
	// ```
	void DeleteMeasurementType(MeasurementTypeID mTypeID);

	// Sets the name of a measurement type
	// @mTypeID the <MeasurementTypeID> to modify
	// @name the wanted name
	//
	// R Version :
	// ```R
	// e$setMeasurementTypeName(mTypeID,"bar")
	// ```
	void SetMeasurementTypeName(MeasurementTypeID mTypeID,
	                            const std::string & name);

	// Gets the Experiment defined measurement type
	//
	// R Version :
	// ```R
	// e$measurementTypeNames()
	// ```
	//
	// @return a map of measurement type name by their <MeasurementTypeID>
	std::map<MeasurementTypeID,std::string> MeasurementTypeNames() const;

	/* cldoc:end-category() */

	/* cldoc:begin-category(ant_interaction) */

	// Creates a new Ant shape type
	// @name the user defined name for the <Ant> Shape Type
	//
	// Creates a new <Ant> virtual shape body part type.
	//
	// R Version :
	// ```R
	// e$createAntShapeType(name)
	// ```
	// @return the <AntShapeTypeID> for the <Ant> shape type
	AntShapeTypeID CreateAntShapeType(const std::string & name);

	// Gets the defined Ant shape type
	//
	// R Version :
	// ```R
	// e$antShapeTypeNames()
	// ```
	//
	// @return the <Ant> shape type name by their <AntShapeTypeID>
	std::map<AntShapeTypeID,std::string> AntShapeTypeNames() const;

	// Changes the name of an Ant Shape type
	// @shapeTypeID the <AntShapeTypeID> of the shape type to rename
	// @name the new name for the <Ant> shape typex
	//
	// R Version :
	// ```R
	// e$setAntShapeTypeName(shapeTypeID,name)
	// ```
	void SetAntShapeTypeName(AntShapeTypeID shapeTypeID,
	                         const std::string & name);

	// Removes a virtual Ant shape type
	// @shapeTypeID the <AntShapeTypeID> of the shape type to remove
	//
	// R Version :
	// ```R
	// e$deleteAntShapeType(shapeTypeID)
	// ```
	void DeleteAntShapeType(AntShapeTypeID shapeTypeID);

	/* cldoc:end-category() */

	/* cldoc:begin-category(named_values) */

	// Adds a non-tracking data column
	// @name the unique name for the column
	// @type the <AntMetadataType> for the column
	// @defaultValue the default value for that column
	//
	// Adds a non-tracking data column with the given <name> and
	// <type>. <name> should be a non-empty unique string for the
	// column.
	//
	// R Version :
	// ```R
	// # adds a column "alive" of type BOOL and defaults to TRUE
	// e$addMetadataColumn("alive",fmAntMetadataType["BOOL"],fmAntStaticBool(TRUE))
	// ```
	void AddMetadataColumn(const std::string & name,
	                       AntMetadataType type,
	                       AntStaticValue defaultValue);

	// Removes a non-tracking data column.
	// @name the name of the column to remove
	//
	// R Version :
	// ```R
	// e$deleteMetadataColumn(name)
	// ```
	void DeleteMetadataColumn(const std::string & name);

	// Gets the types for non-tracking data columns
	//
	// R Version :
	// ```R
	// e$antMetadataColumns()
	// ```
	//
	// @return a pairs of <AntMetadataType> and <AntStaticValue>
	// indexed by column name for all columns defined in the
	// experiment.
	std::map<std::string,std::pair<AntMetadataType,AntStaticValue> > AntMetadataColumns() const;

	// Renames a non-tracking data column
	// @oldName the current name of the column to rename
	// @newName the new wanted name for that column.
	//
	// R Version :
	// ```R
	// e$renameAntMetadataColumn(oldName,newName)
	// ```
	void RenameAntMetadataColumn(const std::string & oldName,
	                             const std::string & newName);

	// Changes the type of a non-tracking data column
	// @name the name of the column
	// @type the new wanted type.
	// @defaultValue the new wantet default value
	//
	// R Version :
	// ```R
	// e$setAntMetadataColumnType("alive",fmAntMetadataType["STRING"],fmAntStaticString("yes"))
	// ```
	void SetAntMetadataColumnType(const std::string & name,
	                              AntMetadataType type,
	                              AntStaticValue defaultValue);

	// Gets tracking data statistics about the Experiment
	//
	// R Version :
	// ```R
	// e$getDataInformations()
	// ```
	//
	// @return an <ExperimentDataInfo> list of informations
	ExperimentDataInfo GetDataInformations() const;

	// Gets AntID <- TagID correspondances at a given time
	// @time the wanted <Time> to query for the correspondances
	// @removeUnidentifiedAnt if `true`, just do not report
	//                        unidentified at this time. If `false`
	//                        `std::numeric_limits<TagID>::max()` will
	//                        be returned as a TagID for unidentified
	//                        Ant (or `NA` for R).
	//
	// R Version
	// ```R
	// # will report NA for unidentified Ant
	// e$identificationsAt(fmTimeParse("2029-11-02T23:42:00.000Z"),FALSE)
	// ```
	//
	// @return a map with the correspondance between AntID and TagID. Unidentified Ant will be ommi
	std::map<AntID,TagID> IdentificationsAt(const Time & time,
	                                        bool removeUnidentifiedAnt = true) const;

	/* cldoc:end-category() */

	// Compiles a TrackingSolver
	//
	// Compiles a <TrackingSolver>, typically use to identify and
	// collide frame from live tracking data.
	//
	// @return a <TrackingSolver> for the experiment.
	TrackingSolver CompileTrackingSolver() const;


	// Opaque pointer to implementation
	typedef const std::shared_ptr<priv::Experiment> PPtr;

	// Private implementation constructor
	// @pExperiment opaque pointer to implementation
	//
	// User cannot create an Experiment directly. They must use
	// <Open>, <OpenReadOnly>, <Create> and <NewFile>.
	Experiment(const PPtr & pExperiment);
private:
	friend class Query;
	PPtr d_p;
};

} //namespace mrymidon
} // namespace fort
