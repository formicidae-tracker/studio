#pragma once

#include "ForwardDeclaration.hpp"

#include <myrmidon/utils/FileSystem.hpp>

#include <google/protobuf/util/time_util.h>

#include <fort-hermes/FileContext.h>

#include "TimeValid.hpp"
#include "SegmentIndexer.hpp"

#include "MovieSegment.hpp"


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
class TrackingDataDirectory : public TimeValid {
public:
	typedef int32_t UID;


	class const_iterator {
	public:
		const_iterator(const fs::path & parentPath,
		               const SegmentIndexer::ConstPtr & segments,
		               uint64_t start,
		               uint64_t end,
		               uint64_t current,
		               UID uid);

		const_iterator(const fs::path & parentPath,
		               const SegmentIndexer::ConstPtr & segments,
		               uint64_t start,
		               uint64_t end,
		               const RawFrameConstPtr & rawFrame,
		               UID uid);

		const_iterator & operator=(const const_iterator & other) = delete;
		const_iterator(const const_iterator & other);

		const_iterator& operator++();
		bool operator==(const const_iterator & other) const;
		bool operator!=(const const_iterator & other) const;
		const RawFrameConstPtr & operator*();
		using difference_type = int64_t;
		using value_type = RawFrameConstPtr;
		using pointer = const RawFrameConstPtr *;
		using reference = const RawFrameConstPtr &;
		using iterator_category = std::forward_iterator_tag;

	private:
		const static RawFrameConstPtr NULLPTR;

		void OpenAt(uint64_t frameID);


		const fs::path           d_parentPath;
		SegmentIndexer::ConstPtr d_segments;
		uint64_t                 d_start,d_end,d_current;
		UID                      d_uid;

		std::unique_ptr<fort::hermes::FileContext> d_file;
		fort::hermes::FrameReadout                 d_message;
		RawFrameConstPtr                           d_frame;
	};


	static UID GetUID(const fs::path & path);

	inline UID GetUID() const {
		return d_uid;
	}

	TrackingDataDirectory();

	TrackingDataDirectory(const fs::path & path,
	                      const fs::path & experimentRoot,
	                      uint64_t startFrame,
	                      uint64_t endFrame,
	                      const Time & start,
	                      const Time & end,
	                      const SegmentIndexer::Ptr & segments,
	                      const MovieSegment::List & movies);


	// Gets the path designating the TrackingDataDirectory
	// @return a path relative to the experiment <Experiment>
	const fs::path & LocalPath() const;

	// Gets the actual path on the filesystem of teh TrackingDataDirectory
	// @return the actual path on the filesystem
	fs::path FilePath() const;

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

	inline const const_iterator & end() const {
		return d_endIterator;
	}

	const_iterator FrameAt(uint64_t frameID) const;

	const_iterator FrameNear(const Time & t) const;

	// Opens an actual TrackingDataDirectory on the filesystem
	// @path path to the tracking data directory.
	// @experimentRoot root of the <Experiment>
	// @return a new <trackingDataDirectory> with all field populated accordingly
	//
	// Opens an actual TrackingDataDirectory on the filesystem, and
	// populate its data form its actual content. This function will
	// look for tracking data file open the first and last segment to
	// obtain infoirmation on the first and last frame.
	static TrackingDataDirectory Open(const fs::path & path, const fs::path & experimentRoot);

	const SegmentIndexer & TrackingIndex() const;

	const MovieSegment::List & MovieSegments() const;

private:
	fs::path       d_experimentRoot, d_path;
	uint64_t       d_startFrame,d_endFrame;

	SegmentIndexer::Ptr d_segments;
	MovieSegment::List  d_movies;

	UID            d_uid;
	const_iterator d_endIterator;

};

} //namespace priv

} //namespace myrmidon

} //namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::TrackingDataDirectory & a);
