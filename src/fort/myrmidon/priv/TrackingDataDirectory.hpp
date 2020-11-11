#pragma once

#include "ForwardDeclaration.hpp"

#include <fort/myrmidon/utils/FileSystem.hpp>

#include <google/protobuf/util/time_util.h>

#include <fort/hermes/FileContext.h>

#include "LocatableTypes.hpp"
#include "TimeValid.hpp"
#include "SegmentIndexer.hpp"
#include "MovieSegment.hpp"
#include "FrameReference.hpp"
#include "TagCloseUp.hpp"
#include "TagStatistics.hpp"


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
class TrackingDataDirectory : public TimeValid, public FileSystemLocatable, public Identifiable {
public:
	typedef std::shared_ptr<TrackingDataDirectory> Ptr;


	typedef int32_t                                    UID;
	typedef SegmentIndexer<std::string>                TrackingIndex;
	typedef SegmentIndexer<MovieSegment::ConstPtr>     MovieIndex;
	typedef std::map<FrameID,FrameReference>           FrameReferenceCache;
	typedef std::shared_ptr<const FrameReferenceCache> FrameReferenceCacheConstPtr;

	class const_iterator {
	public:
		const_iterator(const Ptr & tdd,uint64_t current);

		const const_iterator & operator=(const const_iterator & other) = delete;
		const_iterator & operator=(const_iterator && other);
		const_iterator(const const_iterator & other);
		const_iterator(const_iterator & other);

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
		friend class TrackingDataDirectory;
		const static RawFrameConstPtr NULLPTR;

		void OpenAt(uint64_t frameID);

		Ptr LockParent() const;

		std::weak_ptr<TrackingDataDirectory>       d_parent;
		FrameID d_current;

		std::unique_ptr<fort::hermes::FileContext> d_file;
		fort::hermes::FrameReadout                 d_message;
		RawFrameConstPtr                           d_frame;
	};


	static UID GetUID(const fs::path & absoluteFilePath);

	virtual ~TrackingDataDirectory();

	inline UID GetUID() const {
		return d_uid;
	}

	// The directory path designator
	//
	// Gets the path designating the TrackingDataDirectory
	// @return a path relative to the experiment <Experiment>
	const std::string & URI() const override;


	// The directory absolute path
	//
	// Gets the actual path on the filesystem of the TrackingDataDirectory
	// @return the actual path on the filesystem
	const fs::path & AbsoluteFilePath() const override;

	// Gets the first frame number.
	//
	// @return the first <FrameID> in this directory
	FrameID StartFrame() const;

	// Gets the last frame number
	//
	// @return the last <FrameID> in this directory
	FrameID EndFrame() const;

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

	const_iterator FrameAt(FrameID frameID) const;

	const_iterator FrameAfter(const Time & t) const;

	FrameReference FrameReferenceAt(FrameID frameID) const;

	FrameReference FrameReferenceAfter(const Time & t) const;

	// Opens an actual TrackingDataDirectory on the filesystem
	// @path path to the tracking data directory.
	// @experimentRoot root of the <Experiment>
	// @return a new <trackingDataDirectory> with all field populated accordingly
	//
	// Opens an actual TrackingDataDirectory on the filesystem, and
	// populate its data form its actual content. This function will
	// look for tracking data file open the first and last segment to
	// obtain infoirmation on the first and last frame.
	static TrackingDataDirectory::Ptr Open(const fs::path & TDpath, const fs::path & experimentRoot);

	static TrackingDataDirectory::Ptr Create(const std::string & uri,
	                                         const fs::path & absoluteFilePath,
	                                         uint64_t startFrame,
	                                         uint64_t endFrame,
	                                         const Time & start,
	                                         const Time & end,
	                                         const TrackingIndex::Ptr & segments,
	                                         const MovieIndex::Ptr & movies,
	                                         const FrameReferenceCacheConstPtr & referenceCache);


	const TrackingIndex & TrackingSegments() const;

	const MovieIndex & MovieSegments() const;

	const FrameReferenceCache & ReferenceCache() const;


	class ComputedRessourceUnavailable : public std::runtime_error {
	public:
		ComputedRessourceUnavailable(const std::string & typeName) noexcept;
		virtual ~ComputedRessourceUnavailable() noexcept;
	};

	const std::vector<TagCloseUp::Ptr> & TagCloseUps() const;
	const std::map<FrameReference,fs::path> & FullFrames() const;
	const TagStatisticsHelper::Timed & TagStatistics() const;


	bool TagCloseUpsComputed() const;
	bool TagStatisticsComputed() const;
	bool FullFramesComputed() const;

	typedef std::function<void()> Loader;

	static std::vector<Loader> PrepareTagCloseUpLoaders(const Ptr & itself);
	static std::vector<Loader> PrepareTagStatisticsLoaders(const Ptr & itself);
	static std::vector<Loader> PrepareFullFrameLoaders(const Ptr & itself);


private:

	typedef std::pair<FrameID,Time> TimedFrame;
	typedef std::map<Time::SortableKey,FrameID> FrameIDByTime;


	static void CheckPaths(const fs::path & path,
	                       const fs::path & experimentRoot);

	static void LookUpFiles(const fs::path & absoluteFilePath,
	                        std::vector<fs::path> & hermesFile,
	                        std::map<uint32_t,std::pair<fs::path,fs::path> > & moviesPaths);

	static void LoadMovieSegments(const std::map<uint32_t,std::pair<fs::path,fs::path> > & moviesPaths,
	                              const std::string & parentURI,
	                              MovieSegment::List & movies);

	static TrackingDataDirectory::Ptr LoadFromCache(const fs::path & absoluteFilePath,
	                                                const std::string & URI);

	static std::pair<TimedFrame,TimedFrame>
	BuildIndexes(const std::string & URI,
	             Time::MonoclockID monoID,
	             const std::vector<fs::path> & hermesFile,
	             const TrackingIndex::Ptr & trackingIndexer);

	static void BuildCache(const std::string & URI,
	                       Time::MonoclockID monoID,
	                       const fs::path & tddPath,
	                       const TrackingIndex::ConstPtr & trackingIndexer,
	                       FrameReferenceCache & cache);



	TrackingDataDirectory(const std::string & uri,
	                      const fs::path & absoluteFilePath,
	                      uint64_t startFrame,
	                      uint64_t endFrame,
	                      const Time & start,
	                      const Time & end,
	                      const TrackingIndex::Ptr & segments,
	                      const MovieIndex::Ptr & movies,
	                      const FrameReferenceCacheConstPtr & referenceCache);

	std::map<FrameReference,fs::path> FullFramesFor(const fs::path & subpath) const;

	void ComputeFullFrames() const;

	void SaveToCache() const;

	Ptr Itself() const;


	std::weak_ptr<TrackingDataDirectory> d_itself;

	fs::path       d_absoluteFilePath;
	std::string    d_URI;
	FrameID        d_startFrame,d_endFrame;
	UID            d_uid;
	const_iterator d_endIterator;



	TrackingIndex::Ptr          d_segments;
	MovieIndex::Ptr             d_movies;
	FrameReferenceCacheConstPtr d_referencesByFID;
	FrameIDByTime               d_frameIDByTime;

	// cached data
	std::shared_ptr<std::vector<TagCloseUp::Ptr>>      d_tagCloseUps;
	std::shared_ptr<std::map<FrameReference,fs::path>> d_fullFrames;
	std::shared_ptr<TagStatisticsHelper::Timed>        d_tagStatistics;


};

} //namespace priv
} //namespace myrmidon
} //namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::TrackingDataDirectory & a);
