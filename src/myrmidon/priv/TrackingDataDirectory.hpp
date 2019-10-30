#pragma once

#include "ForwardDeclaration.hpp"

#include <myrmidon/utils/FileSystem.hpp>

#include <google/protobuf/util/time_util.h>

namespace fort {

namespace myrmidon {

namespace pb {
class TrackingDataDirectory;
}

namespace priv {

// Reference to a directory containing tracking data
//
// This object references an actuakl directory on the filesystem that
// contains the tracking data.
//
// Each directory has a start and end time and a start and end frame
class TrackingDataDirectory {
public:
	// Empty undefined constructor
	TrackingDataDirectory();

	// Complete constructor
	// @path relative path from <Experiment::Basedir>
	// @startFrame the first frame number in this directory
	// @endFrame the last frame number contained in this directory
	// @start the time of the first frame in this directory
	// @end the time of the last frame in this directory
	TrackingDataDirectory(const fs::path & path,
	                      uint64_t startFrame,
	                      uint64_t endFrame,
	                      const google::protobuf::Timestamp & start,
	                      const google::protobuf::Timestamp & end);

	// Gets the relative path
	// @return a relative path to this directory from <Experiment::Basedir>
	const fs::path &  Path() const;

	// Gets the first frame number
	// @return the first frame number in this directory
	uint64_t StartFrame() const;

	// Gets the last frame number
	// @return the last frame number in this directory
	uint64_t EndFrame() const;

	// Gets the time of the first frame in this directory
	// @return the time of the first frame in this directory
	const google::protobuf::Timestamp & StartDate() const;

	// Gets the time of the last frame in this directory
	// @return the time of the last frame in this directory
	const google::protobuf::Timestamp & EndDate() const;

	// Creates a FramePointer from this directory
	// @frame the desired frame number
	// @return a FramePointer identifying a frame from this directory
	//
	// Creates a FramePointer from this directory. It will throw
	// std::invalid_argument if <frame> is not in [<Start>,<End>]
	FramePointerPtr FramePointer(uint64_t frame) const;


	// Creates a FramePointer from this directory
	// @frame the desired FramePointer path
	// @return a FramePointer identifying a frame from this directory
	//
	// Creates a FramePointer from this directory by a path of the
	// form `path/frameID`. It will throw std::invalid_argument if
	// `frameID` is not in [<Start>,<End>] or if `path` is different
	// from <Path>.
	FramePointerPtr FramePointer(const fs::path & path) const;


	// Opens an actual TrackingDataDirectory on the filesystem
	// @path path to the tracking data directory
	// @base basepath to sets the relative path. Should use <Experiment::Basedir>
	// @return a new <trackingDataDirectory> with all field populated accordingly
	//
	// Opens an actual TrackingDataDirectory on the filesystem, and
	// populate its data form its actual content. This function will
	// look for tracking data file open the first and last segment to
	// obtain infoirmation on the first and last frame.
	static TrackingDataDirectory Open(const fs::path & path, const fs::path & base);



private:
	fs::path                    d_path;
	uint64_t                    d_startFrame,d_endFrame;
	google::protobuf::Timestamp d_startDate,d_endDate;


};

} //namespace priv

} //namespace myrmidon

} //namespace fort


inline bool operator<(const fort::myrmidon::priv::TrackingDataDirectory & a,
                      const fort::myrmidon::priv::TrackingDataDirectory & b) {
	return a.StartDate() < b.StartDate();
}
