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


/**
 * Entry point of the `fort-myrmidon` API
 *
 * An Experiment olds a collection of Ant, Identification, Space and
 * Zone and give access to the identified tracking data instantaneous
 * collision and interaction detection through Query.
 *
 * File convention
 * ===============
 *
 * Experiment are save to the disk in `.myrmidon` files. One can use
 * Open, OpenDataLess, and Save to interact with these files.
 *
 * Experiment saves relative links to the tracking data directory that
 * are acquired with the FORT system. These paths are relative, so one
 * can rename a `.myrmidon` file on the filesystem with Save, but it must keep
 * the same relative path to the tracking data directories it links to.
 *
 */
class Experiment {
public:
	/** A pointer to an Experiment. */
	typedef std::unique_ptr<Experiment> Ptr;

	/**
	 * Opens an existing Experiment.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.Open(filepath: str) -> py_fort_myrmidon.Experiment
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentOpen <- function(filepath = '') # returns a Rcpp_fmExperiment
	 * ```
	 *
	 * @param filepath the path to the wanted file
	 *
	 * @return a pointer to the Experiment
	 *
	 * @throws std::runtime_error if filepath is not a valid `.myrmidon` file.
	 */
	static Experiment::Ptr Open(const std::string & filepath);

	/**
     * Opens an Experiment without associated tracking data
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.OpenDataLess(filpath: str) -> py_fort_myrmidon.Experiment
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentOpenDataLess <- function(filepath = '') # returns a Rcpp_fmExperiment
	 * ```
	 *
	 * Opens an Experiment to a `.myrmidon` file without opening its
	 * associated tracking data. This is useful, by example, identify
	 * or collides ants from realtime tracking data acquired over the
	 * network using a TrackingSolver obtained with
	 * CompileTrackingSolver(). When opened in 'data-less' mode, no
	 * tracking data, tag statistic or measurement will be returned by
	 * any Query ( the experiment will appear empty ).
	 *
	 * @param filepath the path to the wanted file.
	 *
	 * @return the Experiment
	 *
	 * @throws std::runtime_error if filepath is not a valid
	 *         `.myrmidon` file.
	 */
	static Experiment::Ptr OpenDataLess(const std::string & filepath);

	/**
	 * Creates a new Experiment associated with the given filepath.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.Create(filepath: str) -> py_fort_myrmidon.Experiment
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentCreate <- function(filepath = '') # returns a Rcpp_fmExperiment
	 * ```
	 *
	 * Creates a new Experiment virtually associated with the desired
	 * filepath location. It will not create a new file on the
	 * filesystem. The wanted location is required to compute relative
	 * path to the associated Tracking Data Directory.
	 *
	 * @param filepath the filesystem location for the `.myrmidon` file.
	 *
	 * @return the new empty Experiment
	 */
	static Experiment::Ptr Create(const std::string & filepath);

	/**
	 * Saves the Experiment at the desired filepath
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.Save(self)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentSave <- function(experiment)
	 * ```
	 *
	 * Saves the Experiment to filepath. It is not possible to change
	 * its parent directory (but file renaming is permitted).
	 *
	 * @param filepath the desired filesystem location to save the Experiment to
	 *
	 * @throws std::invalid_argument if `filepath` will change the
	 *         parent directory of the Experiment.
	 */
	void Save(const std::string & filepath);

	/**
	 * Path to the underlying `.myrmidon` file
	 *
	 * * Python: `AbsoluteFilePath :str` read-only property of `py_fort_myrmidon.Experiment`.
	 * R Version :
	 * ```R
	 * fmExperimentAbsoluteFilePath <- function(experiment)
	 * ```
	 *
	 * @return the absolute filepath to the `.myrmidon` file
	 */
	std::string AbsoluteFilePath() const;

	/**
	 * Creates a new Space
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.CreateSpace(self, name: str) -> py_fort_myrmidon.Space
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentCreateSpace <- function(experiment, name = '') # returns a Rcpp_fmSpace
	 * ```
	 *
	 * @param name wanted name for the new Space
	 *
	 * @return the newly created Space.
	 */
	Space::Ptr CreateSpace(const std::string & name);

	/**
	 * Deletes a Space
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.DeleteSpace(self, spaceID: int)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentDeleteSpace <- function(experiment, spaceID = 0)
	 * ```
	 *
	 * @param spaceID the SpaceID of the Space we want to delete.
	 *
	 * @throws std::invalid_argument if spaceID is not a valid ID for
	 *         one of this Experiment Space.
	 */
	void DeleteSpace(SpaceID spaceID);

	/**
	 * Gets Space defined in the Experiment
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.Spaces(self) -> Dict[int,py_fort_myrmidon.Space]
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentSpaces <- function(experiment) # returns a named vector of Rcpp_fmSpace
	 * ```
	 *
	 * @return a map of the Experiment Space indexed by their SpaceID
	 */
	std::map<SpaceID,Space::Ptr> Spaces();

	/**
	 * Adds a tracking data directory to one of Experiment's Space
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.AddTrackingDataDirectory(self,spaceID: int, filepath: str) -> str
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentAddTrackingDataDirectory <- function(experiment,spaceID = 0, filepath = '') # returns a character
	 * ```
	 *
	 * Adds a tracking data director acquired with the FORT to
	 * the wanted Space.
	 *
	 * @param spaceID the Space the data directory should be associated with
	 * @param filepath path to the directory we want to add
	 *
	 * @return the URI used to designate the tdd
	 *
	 * @throws std::runtime_error if the Tracking Data Directory
	 *         contains data that would overlap in time with other
	 *         Tracking Data Directory associated with the same space.
	 */
	std::string AddTrackingDataDirectory(SpaceID spaceID,
	                                     const std::string & filepath);
	/**
	 * Removes a Tracking Data Directory from the Experiment.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.RemoveTrackingDataDirectory(self, URI: str)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentRemoveTrackingDataDirectory <- function(experiment, URI = '')
	 * ```
	 *
	 * @param URI the URI of the tracking data directory to remove
	 *
	 * @throws std::invalid_argument if URI does not designate a
	 *         Tracking Data Directory in the experiment.
	 */
	void RemoveTrackingDataDirectory(const std::string & URI);

	/**
	 * Creates a new Ant in the Experiment.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.CreateAnt(self) -> py_fort_myrmidon.Ant
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentCreateAnt <- function(experiment) # returns a Rcpp_fmAnt
	 * ```
	 *
	 * @return the newly created Ant
	 */
	Ant::Ptr CreateAnt();

	/**
	 * Gets the Ant in the Experiment
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.Ants(self) -> Dict[int,py_fort_myrmidon.Ant]
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentAnts <- function(experiment) # returns a named vector of Rcpp_fmAnts
	 * ```
	 *
	 * @return the Ant indexed by their AntID in the Experiment.
	 */
	std::map<AntID,Ant::Ptr> Ants();

	/**
	 * Adds an Identification to the Experiment
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.AddIdentification(self,antID: int,tagID: int,start: py_fort_myrmidon.Time, end: py_fort_myrmidon.End) -> py_fort_myrmidon.Identification
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentAddIdentification <- function(experiment, antID = 0, tagID = 0, start = fmTimeSinceEver(), end = fmTimeForever() ) # returns a Rcpp_fmIdentification
	 * ```
	 *
	 * Adds an Identification to the Experiment. Identification
	 * are valid for [start,end[. One may obtain a valid time
	 * range using FreeIdentificationRangeAt().
	 *
	 * @param antID the targetted Ant designated by its AntID
	 * @param tagID the tag to associate with the Ant
	 * @param start the first valid Time. It can be Time::SinceEver()
	 * @param end the first invalid Time. It can be Time::Forever()
	 *
	 * @return the new Identification
	 *
	 * @throws OverlapingIdentification if it will conflict in time
	 *         with another Identification with the same antID or
	 *         tagID.
	 */
	Identification::Ptr AddIdentification(AntID antID,
	                                      TagID tagID,
	                                      const Time & start,
	                                      const Time & end);
	/**
	 * Deletes an Identification
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.DeleteIdentification(self,identification: py_fort_myrmidon.Identification)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentDeleteIdentification <- function(experiment, identification)
	 * ```
	 *
	 * @param identification the Identification to delete
	 *
	 * @throws std::invalid_argument if identification is not an
	 *         identification for an Ant of this Experiment.
	 */
	void DeleteIdentification(const Identification::Ptr & identification);

	/**
	 * Computes a valid time range for a tagID.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.FreeIdentificationRangeAt(self,tagID: int, time: py_fort_myrmidon.Time) -> Tuple[py_fort_myrmidon.Time, py_fort_myrmidon.Time]
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentFreeIdentificationRangeAt <- function(experiment, tagID = 0, time = fmTimeSinceEver()) # returns a list of two Rcpp_fmTime.
	 * ```
	 *
	 * @param tagID the TagID we want a range for
	 * @param time the Time that must be included in the result time range
	 *
	 * Queries for a valid time range for a given TagID and
	 * Time. The result will be a range [start,end[ containing
	 * time where tagID is not used to identify any Ant.
	 *
	 * @return two Time that represents a valid [start,end[ range for tagID
	 *
	 * @throws std::runtime_error if tagID already identifies an Ant at time.
	 */
	std::tuple<fort::Time,fort::Time> FreeIdentificationRangeAt(TagID tagID,
	                                                            const Time & time) const;

	/**
	 * The name of the Experiment.
	 *
	 * * Python: `Name :str` read-write property of `py_fort_myrmidon.Experiment`
	 * * R :
	 * ```R
	 * fmExperimentName <- function(experiment) # returns a character
	 * ```
	 *
	 * @return a reference to the Experiment's name
	 */
	const std::string & Name() const;

	/**
	 * Sets the Experiment's name.
	 *
	 * * Python: `Name :str` read-write property of `py_fort_myrmidon.Experiment`
	 * * R :
	 * ```R
	 * fmExperimentSetName <- function(experiment, name = '')
	 * ```
	 *
	 * @param name the new Experiment name
	 */
	void SetName(const std::string & name);

	/**
	 * The author of the Experiment
	 *
	 * * Python: `Author :str` read-write property of `py_fort_myrmidon.Experiment`
	 * * R :
	 * ```R
	 * fmExperimentAuthor <- function(experiment) # return a character
	 * ```
	 *
	 * @return a reference to the Experiment's author name
	 */
	const std::string & Author() const;

	/**
	 * Sets the Experiment's author
	 *
	 * * Python: `Author :str` read-write property of `py_fort_myrmidon.Experiment`
	 * * R :
	 * ```R
	 * fmExperimentSetAuthor <- function(experiment, author = '')
	 * ```
	 *
	 * @param author the new value for the Experiment's author
	 */
	void SetAuthor(const std::string & author);

	/**
	 * Comments about the experiment
	 *
	 * * Python: `Comment :str` read-write property of `py_fort_myrmidon.Experiment`
	 * * R :
	 * ```R
	 * fmExperimentComment <- function(experiment) # return a character
	 * ```
	 *
	 * @return a reference to the Experiment's comment
	 */
	const std::string & Comment() const;

	/**
	 * Sets the comment of the Experiment
	 *
	 * * Python: `Comment :str` read-write property of `py_fort_myrmidon.Experiment`
	 * * R :
	 * ```R
	 * fmExperimentSetComment <- function(experiment, comment = '')
	 * ```
	 *
	 * @param comment the wanted Experiment's comment
	 */
	void SetComment(const std::string & comment);

	/**
	 * The kind of tag used in the Experiment
	 *
	 * * Python: `TagFamily :py_fort_myrmidon.TagFamily` read-only property of `py_fort_myrmidon.Experiment`
	 * * R:
	 * ```R
	 * fmExperimentTagFamily <- function(experiment) # return an integer corresponding to one of the value of `fmTagFamily` named list
	 * ```
	 *
	 * Gets the family of the tags used in this Experiment. It is
	 * automatically determined from the information in
	 * TrackingDataDirectory.
	 *
	 * @return the family of tag used in the Experiment
	 */
	fort::tags::Family Family() const;

	/**
	 * The default physical tag size
	 *
	 * * Python: `DefaultTagSize :float` read-write property of `py_fort_myrmidon.Experiment`
	 * * R :
	 * ```R
	 * fmExperimentDefaultTagSize <- function(experiment) # returns a numerical
	 * ```
	 *
	 * Usually an Ant colony are tagged with a majority of tag of a
	 * given size in millimeters. Some individuals (like Queens) may
	 * often use a bigger tag size that should individually be set in
	 * their Identification. This value is then used for
	 * Query::ComputeMeasurementFor.
	 *
	 * `fort-myrmidon` uses without white border convention for ARTag and
	 * with white border convention Apriltag.
	 *
	 * @return the default tag size for the Experiment in millimeters
	 */
	double DefaultTagSize() const;

	/**
	 * Sets the default tag siye in mm
	 *
	 * * Python: `DefaultTagSize :float` read-write property of `py_fort_myrmidon.Experiment`
	 * * R :
	 * ```R
	 * fmExperimentSetDefaultTagSize <- function(experiment, tagSize = 1.0)
	 * ```
	 *
	 * @param defaultTagSize the tag size in millimeter ( the one defined on the tag sheet )
	 *
	 */
	void SetDefaultTagSize(double defaultTagSize);

	/**
	 * Creates a measurement type
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.CreateMeasurementType(self,name: str) -> int
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentCreateMeasurementType <- function(experiment, name = '') # returns an integer
	 * ```
	 *
	 * @param name the wanted name for the new measurement
	 *
	 * @return the MeasurementTypeID identifying the new measurement
	 *         type
	 */
	MeasurementTypeID CreateMeasurementType(const std::string & name);

	/**
	 * Deletes a measurement type
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.DeleteMeasurementType(self,measurementTypeID :int)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentDeleteMeasurementType <- function(experiment, measurementTypeID = 0)
	 * ```
	 *
	 * @param measurementTypeID the MeasurementTypeID to delete
	 *
	 * @throws std::invalid_argument if measurementTypeID is not valid
	 *         for this Experiment.
	 */
	void DeleteMeasurementType(MeasurementTypeID measurementTypeID);

	/**
	 * Sets the name of a measurement type
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.SetMeasurementTypeName(self,measurementTypeID :int, name :str)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentSetMeasurementTypeName <- function(experiment, measurementTypeID = 0, name :str)
	 * ```
	 *
	 * @param measurementTypeID the MeasurementTypeID to modify
	 * @param name the wanted name
	 *
	 * @throws std::invalid_argument if measurementTypeID is not valid
	 *         for this Experiment.
	 */
	void SetMeasurementTypeName(MeasurementTypeID measurementTypeID,
	                            const std::string & name);

	/**
	 * Gets the Experiment defined measurement types
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.MeasurementTypeNames(self) -> Dict[int,str]
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentMeasurementTypeNames <- function(experiment) # returns a named vector of the measurement type names
	 * ```
	 *
	 * @return a map of measurement type name by their MeasurementTypeID
	 */
	std::map<MeasurementTypeID,std::string> MeasurementTypeNames() const;

	/**
	 * Creates a new Ant shape type
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.CreateAntShapeType(self, name: str) -> int
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentCreateAntShapeType <- function(experiment, name = '') # returns an integer
	 * ```
	 *
	 * @param name the user defined name for the Ant Shape Type
	 *
	 * @return the AntShapeTypeID identifying the new Ant shape type.
	 */
	AntShapeTypeID CreateAntShapeType(const std::string & name);

	/**
	 * Gets the defined Ant shape type
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.AntShapeTypeNames(self) -> Dict[int,str]
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentAntShapeTypeNames <- function(experiment) # returns a named vector of the ant shape type names
	 * ```
	 *
	 * @return the Ant shape type name by their AntShapeTypeID
	 */
	std::map<AntShapeTypeID,std::string> AntShapeTypeNames() const;

	/**
	 * Changes the name of an Ant Shape type
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.SetAntShapeTypeName(self, shapeTypeID :int, name :str)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentSetAntShapeTypeName <- function(experiment, shapeTypeID = 0, name = '')
	 * ```
	 *
	 * @param shapeTypeID the AntShapeTypeID of the shape type to rename
	 * @param name param the new name for the Ant shape type
	 *
	 * @throws std::invalid_argument if shapeTypeID is not valid for
	 *         this Experiment.
	 */
	void SetAntShapeTypeName(AntShapeTypeID shapeTypeID,
	                         const std::string & name);

	/**
	 * Removes a virtual Ant shape type
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.RemoveAntShapeType(self, shapeTypeID :int)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentRemoveAntShapeTypeName <- function(experiment, shapeTypeID = 0)
	 * ```
	 *
	 * @param shapeTypeID the AntShapeTypeID of the shape type to remove
	 *
	 * @throws std::invalid_argument if shapeTypeID is not valid for
	 *         this Experiment.
	 */
	void DeleteAntShapeType(AntShapeTypeID shapeTypeID);

	/**
	 * Adds or modify a user-defined meta data key.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.SetMetaDataKey(self, key :str, value: py_fort_myrmidon.AntStaticValue)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentAddMetaDataKey <- function(experiment, key = '', value = fmAntStaticBool(FALSE) )
	 * ```
	 *
	 * @param key the unique key to add or modify
	 * @param defaultValue the default value for that key. It also
	 *        determines the type for the key.
	 *
	 * Adds a non-tracking data key with the given name, type and
	 * defaultValue.
	 *
	 */
	void SetMetaDataKey(const std::string & key,
	                    AntStaticValue defaultValue);

	/**
	 * Removes a meta data key.
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.DeleteMetaDataKey(self, key :str)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentDeleteMetaDataKey <- function(experiment, key = '')
	 * ```
	 *
	 * @param key the key to remove
	 *
	 * @throws std::invalid_argument if key is not valid for this
	 *         Experiment.
	 */
	void DeleteMetaDataKey(const std::string & key);

	/**
	 * Gets the meta data keys for this Experiment
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.MetaDataKeys(self) -> Dict[str,Tuple[py_fort_myrmidon.AntMetaDataType,py_fort_myrmidon.AntStaticValue]]
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentMetaDataKeys <- function(experiment) # returns a named vector
	 * ```
	 *
	 * @return a pairs of AntMetadataType and AntStaticValue
	 *          indexed by key
	 */
	std::map<std::string,std::pair<AntMetaDataType,AntStaticValue> > MetaDataKeys() const;

	/**
	 * Renames a meta data key
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.RenameMetaDataKey(self, oldKey :str, newKey :str)
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentRenameMetaDataKey <- function(experiment, oldKey = '', newKey = '')
	 * ```
	 *
	 * @param oldKey the key to rename
	 * @param newKey the new key name
	 *
	 *
	 * @throws std::invalid_argument if key is not valid for this
	 *         Experiment.
	 */
	void RenameMetaDataKey(const std::string & oldKey,
	                       const std::string & newKey);

	/**
	 * Gets tracking data statistics about the Experiment
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.GetDataInformations(self) -> py_fort_myrmidon.ExperimentDataInfo
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentGetDataInformations <- function(experiment) # a list of number of frames, and start and end date of the experiment.
	 * ```
	 *
	 * @return an ExperimentDataInfo structure of informations
	 */
	ExperimentDataInfo GetDataInformations() const;

	/**
	 * Gets AntID <- TagID correspondances at a given time
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.IdentificationsAt(self, time :py_fort_myrmidon.Time) -> Dict[int,int]
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentIdentificationsAt <- function(experiment, time = fmTimeNow()) # named vector of antID
	 * ```
	 *
	 * @param time the wanted Time to query for the correspondances
	 * @param removeUnidentifiedAnt if `true`, just do not report
	 *        unidentified at this time. If `false`
	 *        std::numeric_limits<TagID>::max() will be returned as
	 *        a TagID for unidentified Ant (or `NA` for R).
	 *
	 * @return a map with the correspondance between AntID and TagID.
	 */
	std::map<AntID,TagID> IdentificationsAt(const Time & time,
	                                        bool removeUnidentifiedAnt) const;

	/**
	 * Compiles a TrackingSolver
	 *
	 * * Python:
	 * ```python
	 * py_fort_myrmidon.Experiment.CompileTrackingSolver(self) -> py_fort_myrmidon.TrackingSolver
	 * ```
	 * * R :
	 * ```R
	 * fmExperimentCompileTrackingSolver <- function(experiment) # returns a Rcpp_fmTrackingSolver
	 * ```
	 *
	 * Compiles a TrackingSolver, typically use to identify and
	 * collide frame from online acquired tracking data.
	 *
	 * @return a TrackingSolver for the Experiment.
	 */
	TrackingSolver CompileTrackingSolver() const;


private:
	friend class Query;

	// Opaque pointer to implementation
	typedef const std::shared_ptr<priv::Experiment> PPtr;

	// Private implementation constructor
	// @pExperiment opaque pointer to implementation
	//
	// User cannot create an Experiment directly. They must use
	// <Open>, <OpenReadOnly>, <Create> and <NewFile>.
	Experiment(const PPtr & pExperiment);


	Experiment & operator=(const Experiment &) = delete;
	Experiment(const Experiment &) = delete;

	PPtr d_p;
};

} // namespace mrymidon
} // namespace fort
