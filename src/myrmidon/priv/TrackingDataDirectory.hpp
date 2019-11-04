#pragma once

#include "ForwardDeclaration.hpp"

#include <myrmidon/utils/FileSystem.hpp>

#include <google/protobuf/util/time_util.h>

#include <fort-hermes/FileContext.h>

#include "SegmentIndexer.hpp"

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
	typedef int32_t UID;


	class const_iterator {
	public:
		const_iterator(std::unique_ptr<fort::hermes::FileContext> & fc);
		const_iterator& operator++();
		bool operator==(const const_iterator & other);
		bool operator!=(const const_iterator & other);
		RawFrameConstPtr operator*();
		using difference_type = int64_t;
		using value_type = RawFrameConstPtr;
		using pointer = const RawFrameConstPtr *;
		using reference = const RawFrameConstPtr &;
		using iterator_category = std::forward_iterator_tag;

	private:
		const std::unique_ptr<fort::hermes::FileContext> d_file;
		fort::hermes::FrameReadout                       d_message;
	};


	static UID GetUID(const fs::path & path,  const fs::path & base);

	TrackingDataDirectory();

	TrackingDataDirectory(const fs::path & path,
	                      uint64_t startFrame,
	                      uint64_t endFrame,
	                      const Time & start,
	                      const Time & end,
	                      const SegmentIndexer & segments);


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
	const Time & StartDate() const;

	// Gets the time of the last frame in this directory
	// @return the time of the last frame in this directory
	const Time & EndDate() const;


	const_iterator begin() const;
	const_iterator end() const;

	const_iterator FrameAt(uint64_t frameID) const;

	const_iterator FrameNear(const Time & t) const;

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

	const SegmentIndexer & TrackingIndex() const;


private:
	fs::path       d_path;
	uint64_t       d_startFrame,d_endFrame;
	Time           d_startDate,d_endDate;
	SegmentIndexer d_segments;

};

} //namespace priv

} //namespace myrmidon

} //namespace fort


inline bool operator<(const fort::myrmidon::priv::TrackingDataDirectory & a,
                      const fort::myrmidon::priv::TrackingDataDirectory & b) {
	return a.StartDate().Before(b.StartDate());
}
