#include "TrackingDataDirectory.hpp"

#include <mutex>
#include <regex>

#include <tbb/parallel_for.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include <yaml-cpp/yaml.h>

#include <fort/hermes/Error.h>
#include <fort/hermes/FileContext.h>

#include <fort/myrmidon/utils/NotYetImplemented.hpp>
#include <fort/myrmidon/utils/Checker.hpp>
#include <fort/myrmidon/utils/Defer.hpp>
#include <fort/myrmidon/utils/ObjectPool.hpp>
#include <fort/myrmidon/priv/proto/TDDCache.hpp>
#include <fort/myrmidon/priv/proto/TagStatisticsCache.hpp>
#include <fort/myrmidon/priv/proto/TagCloseUpCache.hpp>

#include "TagCloseUp.hpp"
#include "TimeUtils.hpp"
#include "RawFrame.hpp"



#ifdef MYRMIDON_USE_BOOST_FILESYSTEM
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::regular_file)
#else
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::file_type::regular)
#endif


namespace fort {
namespace myrmidon {
namespace priv {

TrackingDataDirectory::~TrackingDataDirectory() {}


TrackingDataDirectory::Ptr TrackingDataDirectory::Create(const std::string & uri,
                                                         const fs::path & absoluteFilePath,
                                                         uint64_t startFrame,
                                                         uint64_t endFrame,
                                                         const Time & startdate,
                                                         const Time & enddate,
                                                         const TrackingIndex::Ptr & si,
                                                         const MovieIndex::Ptr & movies,
                                                         const FrameReferenceCacheConstPtr & referenceCache) {

	FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(absoluteFilePath);

	std::shared_ptr<TrackingDataDirectory> res(new TrackingDataDirectory(uri,
	                                                                     absoluteFilePath,
	                                                                     startFrame,
	                                                                     endFrame,
	                                                                     startdate,
	                                                                     enddate,
	                                                                     si,
	                                                                     movies,
	                                                                     referenceCache));
	res->d_itself = res;
	res->d_endIterator.d_parent = res;
	return res;
}


TrackingDataDirectory::TrackingDataDirectory(const std::string & uri,
                                             const fs::path & absoluteFilePath,
                                             uint64_t startFrame,
                                             uint64_t endFrame,
                                             const Time & startdate,
                                             const Time & enddate,
                                             const TrackingIndex::Ptr & si,
                                             const MovieIndex::Ptr & movies,
                                             const FrameReferenceCacheConstPtr & referenceCache)
	: d_absoluteFilePath(absoluteFilePath)
	, d_URI(uri)
	, d_startFrame(startFrame)
	, d_endFrame(endFrame)
	, d_uid(GetUID(d_absoluteFilePath))
	, d_endIterator(Ptr(),endFrame+1)
	, d_segments(si)
	, d_movies(movies)
	, d_referencesByFID(referenceCache) {

	d_start = std::make_shared<const Time>(startdate);
	d_end = std::make_shared<const Time>(enddate);
	if ( d_startFrame >= d_endFrame ) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startFrame:" << d_startFrame << " >= endDate: " << d_endFrame;
		throw std::invalid_argument(os.str());
	}

	if ( d_start->Before(*d_end) == false ) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startDate:" << *d_start << " >= endDate: " << *d_end;
		throw std::invalid_argument(os.str());
	}

	for ( const auto & [frameID,ref] : *referenceCache ) {
		d_frameIDByTime.insert(std::make_pair(ref.Time().SortKey(),frameID));
	}

}


const std::string & TrackingDataDirectory::URI() const {
	return d_URI;
}

const fs::path & TrackingDataDirectory::AbsoluteFilePath() const {
	return d_absoluteFilePath;
}

uint64_t TrackingDataDirectory::StartFrame() const {
	return d_startFrame;
}

uint64_t TrackingDataDirectory::EndFrame() const {
	return d_endFrame;
}

const Time & TrackingDataDirectory::StartDate() const {
	return *d_start;
}

const Time & TrackingDataDirectory::EndDate() const {
	return *d_end;
}

TrackingDataDirectory::UID TrackingDataDirectory::GetUID(const fs::path & filepath) {
	static std::mutex mutex;
	static UID last = 0;
	static std::map<fs::path,UID> d_UIDs;
	std::lock_guard<std::mutex> lock(mutex);
	fs::path fpath = fs::weakly_canonical(fs::absolute(filepath));
	auto fi = d_UIDs.find(fpath);
	if ( fi == d_UIDs.end() ) {
		d_UIDs.insert(std::make_pair(fpath,++last));
		return last;
	}
	return fi->second;
}


const tags::ApriltagOptions & TrackingDataDirectory::DetectionSettings() const  {
	return d_detectionSettings;
}

void TrackingDataDirectory::CheckPaths(const fs::path & path,
                                       const fs::path & experimentRoot) {
	if ( fs::is_directory(experimentRoot) == false ) {
		throw std::invalid_argument("experiment root path " + experimentRoot.string() +  " is not a directory");
	}
	if ( fs::is_directory(path) == false ) {
		throw std::invalid_argument( path.string() + " is not a directory");
	}
}


void TrackingDataDirectory::LookUpFiles(const fs::path & absoluteFilePath,
                                        std::vector<fs::path> & hermesFiles,
                                        std::map<uint32_t,std::pair<fs::path,fs::path> > & moviesPaths) {
	auto extractID =
		[](const fs::path & p) -> uint32_t {
			std::istringstream iss(p.stem().extension().string());
			uint32_t res;
			iss.ignore(std::numeric_limits<std::streamsize>::max(),'.');
			iss >> res;
			if (!iss) {
				throw std::runtime_error("Could not extract id in " +p.string());
			}
			return res;
		};
	for( auto const & f : fs::directory_iterator(absoluteFilePath) ) {
		if ( ! MYRMIDON_FILE_IS_REGULAR(f.status()) ) {
			continue;
		}
		auto p = f.path();
		if ( p.extension() == ".hermes") {
			hermesFiles.push_back(p);
			continue;
		}

		if ( p.extension() == ".mp4" && p.stem().stem() == "stream" ) {
			moviesPaths[extractID(p)].first = p;
		}

		if ( p.extension() == ".txt" && p.stem().stem() == "stream.frame-matching" ) {
			moviesPaths[extractID(p)].second = p;
		}
	}

	std::sort(hermesFiles.begin(),hermesFiles.end());

}

void TrackingDataDirectory::LoadMovieSegments(const std::map<uint32_t,std::pair<fs::path,fs::path> > & moviesPaths,
                                              const std::string & parentURI,
                                              MovieSegment::List & movies ){
	for ( const auto & [id,paths] : moviesPaths ) {
		if ( !paths.first.empty() && !paths.second.empty() ) {
			movies.push_back(MovieSegment::Open(id,paths.first,paths.second,parentURI));
		}
	}

	std::sort(movies.begin(),movies.end(),[](const MovieSegment::Ptr & a,
	                                         const MovieSegment::Ptr & b) {
		                                      return a->StartFrame() < b->StartFrame();
	                                      });
}

void TrackingDataDirectory::BuildFrameReferenceCache(const std::string & URI,
                                                     Time::MonoclockID monoID,
                                                     const fs::path & tddPath,
                                                     const TrackingIndex::ConstPtr & trackingIndexer,
                                                     FrameReferenceCache & cache) {
	struct CacheSegment {
		std::string AbsoluteFilePath;
		std::set<FrameID> ToFind;
		std::vector<Time>    Times;
		void Load(Time::MonoclockID monoID) {
			Times.clear();
			Times.reserve(ToFind.size());

			fort::hermes::FileContext fc(AbsoluteFilePath,false);
			fort::hermes::FrameReadout ro;
			bool first = true;
			for ( auto iter = ToFind.cbegin(); iter != ToFind.cend() ;) {
				try {
					fc.Read(&ro);
					FrameID curFrameID = ro.frameid();
					Time curTime = TimeFromFrameReadout(ro,monoID);
					if ( *iter == curFrameID ) {
						Times.push_back(curTime);
						++iter;
					}
				} catch ( const fort::hermes::EndOfFile & ) {
					if ( iter != ToFind.end() ) {
						throw std::runtime_error("Frame "
						                         + std::to_string(*iter)
						                         + " is outside of file "
						                         + AbsoluteFilePath);
					}
				} catch ( const std::exception & e ) {
					throw std::runtime_error("[TDD.BuildCache]: Could not find frame "
					                         + std::to_string(*iter) + ": " +  e.what());
				}
			}
		}
	};

	std::map<std::string,CacheSegment> toFind;
	std::vector<CacheSegment> flattened;
	for ( const auto & [frameID,neededRef] : cache ) {
		const auto & [ref,file] = trackingIndexer->Find(frameID);
		toFind[file].ToFind.insert(frameID);
		toFind[file].ToFind.insert(ref.FrameID());
	}
	flattened.reserve(toFind.size());
	for ( auto & [file,segment] : toFind ) {
		segment.AbsoluteFilePath = (tddPath / file).string();
		flattened.push_back(segment);
	}

	// do the parrallel computations
	tbb::parallel_for(tbb::blocked_range<size_t>(0,flattened.size()),
	                  [&flattened,monoID](const tbb::blocked_range<size_t> & range) {
		                  for ( size_t idx = range.begin();
		                        idx != range.end();
		                        ++idx ) {
			                  flattened[idx].Load(monoID);
		                  }
	                  });
	// merge all
	for ( const auto & segment : flattened ) {
		size_t i = 0;
		for ( const auto & frameID : segment.ToFind ) {
			const auto & time = segment.Times[i];
			cache[frameID] = FrameReference(URI,frameID,time);
			++i;
		}
	}

}


std::pair<TrackingDataDirectory::TimedFrame,TrackingDataDirectory::TimedFrame>
TrackingDataDirectory::BuildIndexes(const std::string & URI,
                                    Time::MonoclockID monoID,
                                    const std::vector<fs::path> & hermesFiles,
                                    const TrackingIndex::Ptr & trackingIndexer) {

	uint64_t start,end;
	Time startDate,endDate;

	fort::hermes::FrameReadout ro;
	bool first = true;
	std::shared_ptr<fort::hermes::FileContext> fc;

	for (const auto & f : hermesFiles ) {
		try {
			fc = std::make_shared<fort::hermes::FileContext>(f.string());
			fc->Read(&ro);
			Time startTime = TimeFromFrameReadout(ro,monoID);

			if ( first == true) {
				start = ro.frameid();
				startDate = startTime;
				first = false;
			}

			FrameID curFrameID = ro.frameid();
			FrameReference curReference(URI,curFrameID,startTime);
			trackingIndexer->Insert(curReference,
			                        f.filename().generic_string());
		} catch ( const std::exception & e) {
			throw std::runtime_error("Could not extract frame from " +  f.string() + ": " + e.what());
		}
	}

	try {
		for (;;) {
			fc->Read(&ro);
			end = ro.frameid();
			endDate = TimeFromFrameReadout(ro,monoID);

			//we add 1 nanosecond to transform the valid range from
			//[start;end[ to [start;end] by making it
			//[start;end+1ns[. There are no time existing between end
			//and end+1ns;
			endDate = endDate.Add(1);
		}
	} catch ( const fort::hermes::EndOfFile &) {
		//DO nothing, we just reached EOF
	} catch ( const std::exception & e) {
		throw std::runtime_error("Could not extract last frame from " +  hermesFiles.back().string() + ": " + e.what());
	}



	return std::make_pair(std::make_pair(start,startDate),
	                      std::make_pair(end,endDate));

}


std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>>
TrackingDataDirectory::ListTagCloseUpFiles(const fs::path & path) {
	PERF_FUNCTION();
	std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>> res;

	static std::regex singleRx("ant_([0-9]+)_frame_([0-9]+).png");
	static std::regex multiRx("frame_([0-9]+).png");

	for ( const auto & de : fs::directory_iterator(path) ) {
		auto ext = de.path().extension().string();
		std::transform(ext.begin(),ext.end(),ext.begin(),
		               [](unsigned char c) {
			               return std::tolower(c);
		               });
		if ( ext != ".png" ) {
			continue;
		}

		std::smatch ID;
		std::string filename = de.path().filename().string();
		FrameID frameID;
		if(std::regex_search(filename,ID,singleRx) && ID.size() > 2) {
			std::istringstream IDS(ID.str(1));
			std::istringstream FrameS(ID.str(2));
			auto tagID = std::make_shared<TagID>(0);

			IDS >> *(tagID);
			FrameS >> frameID;
			res.insert(std::make_pair(frameID,std::make_pair(de.path(),tagID)));
			continue;
		}
		if(std::regex_search(filename,ID,multiRx) && ID.size() > 1) {
			std::istringstream FrameS(ID.str(1));
			FrameS >> frameID;
			res.insert(std::make_pair(frameID,std::make_pair(de.path(),std::shared_ptr<TagID>())));
			continue;
		}

	}

	return res;
}



TrackingDataDirectory::Ptr TrackingDataDirectory::Open(const fs::path & filepath, const fs::path & experimentRoot) {
	CheckPaths(filepath,experimentRoot);

	auto absoluteFilePath = fs::weakly_canonical(fs::absolute(filepath));
	auto URI = fs::relative(absoluteFilePath,fs::absolute(experimentRoot));

	Ptr res;

	bool cacheData = false;

	try {
		res = LoadFromCache(absoluteFilePath,URI.generic_string());
	} catch (const std::exception & e ) {
		res = OpenFromFiles(absoluteFilePath,URI.generic_string());
		cacheData = true;
	}

	if ( cacheData == true ) {
		try {
			res->SaveToCache();
		} catch ( const std::exception & e) {}
	}

	res->LoadComputedFromCache();
	res->LoadDetectionSettings();

	return res;
}


TrackingDataDirectory::Ptr TrackingDataDirectory::OpenFromFiles(const fs::path & absoluteFilePath,
                                                                const std::string & URI) {


	std::vector<fs::path> hermesFiles;
	std::map<uint32_t,std::pair<fs::path,fs::path> > moviesPaths;
	MovieSegment::List movies;
	auto ti = std::make_shared<TrackingIndex>();
	auto mi = std::make_shared<MovieIndex>();
	auto referenceCache = std::make_shared<FrameReferenceCache>();

	LookUpFiles(absoluteFilePath,hermesFiles,moviesPaths);
	if ( hermesFiles.empty() ) {
		throw std::invalid_argument(absoluteFilePath.string() + " does not contains any .hermes file");
	}

	LoadMovieSegments(moviesPaths,URI,movies);
	for(const auto & m : movies) {
		referenceCache->insert(std::make_pair(m->StartFrame(),FrameReference(URI,0,Time())));
		referenceCache->insert(std::make_pair(m->EndFrame(),FrameReference(URI,0,Time())));
	}

	auto snapshots = ListTagCloseUpFiles(absoluteFilePath / "ants");
	for(const auto & [frameID,s] : snapshots) {
		referenceCache->insert(std::make_pair(frameID,FrameReference(URI,0,Time())));
	}

	Time::MonoclockID monoID = GetUID(absoluteFilePath);

	auto bounds = BuildIndexes(URI,
	                           monoID,
	                           hermesFiles,
	                           ti);

	BuildFrameReferenceCache(URI,
	                         monoID,
	                         absoluteFilePath,
	                         ti,
	                         *referenceCache);
	// caches the last frame
	referenceCache->insert(std::make_pair(bounds.second.first,
	                                      FrameReference(URI,
	                                                     bounds.second.first,
	                                                     bounds.second.second.Add(-1))));
	Time emptyTime;

	for(const auto & m : movies) {
		auto fi = referenceCache->find(m->StartFrame());
		if (fi == referenceCache->cend() ||
		    ( fi->second.FrameID() == 0 && fi->second.Time().Equals(emptyTime) ) ) {
			std::ostringstream oss;
			oss << "[MovieIndexing] Could not find FrameReference for FrameID " << m->StartFrame();
			throw std::logic_error(oss.str());
		}
		mi->Insert(fi->second,m);
	}

	std::vector<FrameID> toErase;
	toErase.reserve(referenceCache->size());
	for ( const auto & [frameID,ref] : *referenceCache ) {
		if (ref.FrameID() == 0 && ref.Time().Equals(emptyTime) ) {
			toErase.push_back(frameID);
		}
	}

	for( auto frameID : toErase ) {
		std::cerr << "[CacheCleaning] Could not find FrameReference for FrameID " << frameID << std::endl;
		referenceCache->erase(frameID);
	}

	return TrackingDataDirectory::Create(URI,
	                                     absoluteFilePath,
	                                     bounds.first.first,
	                                     bounds.second.first,
	                                     bounds.first.second,
	                                     bounds.second.second,
	                                     ti,
	                                     mi,
	                                     referenceCache);
}


const TrackingDataDirectory::TrackingIndex &
TrackingDataDirectory::TrackingSegments() const {
	return *d_segments;
}


TrackingDataDirectory::const_iterator::const_iterator(const TrackingDataDirectory::Ptr & parent,
                                                      uint64_t current)
	: d_parent(parent)
	, d_current(current) {
}

TrackingDataDirectory::const_iterator::const_iterator(const_iterator & other)
	: d_parent(other.d_parent)
	, d_current(other.d_current)
	, d_file(std::move(other.d_file))
	, d_message(other.d_message)
	, d_frame(other.d_frame) {
}

TrackingDataDirectory::const_iterator::const_iterator(const const_iterator & other)
	: d_parent(other.d_parent)
	, d_current(other.d_current) {
}

TrackingDataDirectory::const_iterator &
TrackingDataDirectory::const_iterator::operator=(const_iterator && other) {
	d_parent = other.d_parent;
	d_current = other.d_current;
	d_file = std::move(other.d_file);
	d_message = other.d_message;
	d_frame = (other.d_frame);
	return *this;
}

TrackingDataDirectory::const_iterator& TrackingDataDirectory::const_iterator::operator++() {
	auto parent = LockParent();
	if ( d_current <= parent->d_endFrame ) {
		++d_current;
	}
	return *this;
}

bool TrackingDataDirectory::const_iterator::operator==(const const_iterator & other) const {
	auto parent = LockParent();
	auto otherParent = other.LockParent();
	return (parent->GetUID() == otherParent->GetUID()) && (d_current == other.d_current);
}

bool TrackingDataDirectory::const_iterator::operator!=(const const_iterator & other) const {
	return !(*this == other);
}

const RawFrameConstPtr TrackingDataDirectory::const_iterator::NULLPTR;

const RawFrameConstPtr & TrackingDataDirectory::const_iterator::operator*() {
	auto parent = LockParent();
	if ( d_current > parent->d_endFrame ) {
		return NULLPTR;
	}
	while ( !d_frame || d_frame->Frame().FrameID() < d_current) {
		if ( !d_file ) {
			auto p = parent->d_absoluteFilePath / parent->d_segments->Find(d_current).second;
			d_file = std::unique_ptr<fort::hermes::FileContext>(new fort::hermes::FileContext(p.string()));
			d_message.Clear();
		}

		try {
			d_file->Read(&d_message);
			d_frame = RawFrame::Create(parent->d_URI,d_message,parent->d_uid);

		} catch( const fort::hermes::EndOfFile & ) {
			d_current = parent->d_endFrame + 1;
			d_frame.reset();
			return NULLPTR;
		}
	}
	if ( d_frame->Frame().FrameID() > d_current ) {
		d_current = d_frame->Frame().FrameID();
	}
	return d_frame;
}


TrackingDataDirectory::Ptr TrackingDataDirectory::const_iterator::LockParent() const {
	if ( auto locked = d_parent.lock() ) {
		return locked;
	}
	throw DeletedReference<TrackingDataDirectory>();
}


TrackingDataDirectory::const_iterator TrackingDataDirectory::begin() const {
	return const_iterator(Itself(),d_startFrame);
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameAt(uint64_t frameID) const {
	if ( frameID < d_startFrame || frameID > d_endFrame ) {
		std::ostringstream os;
		os << "Could not find frame " << frameID << " in [" << d_startFrame << ";" << d_endFrame << "]";
		throw std::out_of_range(os.str());
	}
	return const_iterator(Itself(),frameID);
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameAfter(const Time & t) const {
	if ( t < StartDate() ) {
		std::ostringstream oss;
		oss << t << " is not in ["
		    << StartDate() << ",+∞[";
		throw std::out_of_range(oss.str());
	}
	auto iter = FrameAt(d_segments->Find(t).first.FrameID());
	Time curTime = (*iter)->Frame().Time();
	if (curTime == t) {
		return iter;
	}
	for ( ; iter != end(); ++iter) {
		curTime = (*iter)->Frame().Time();
		if ( curTime >= t ) {
			return iter;
		}
	}
	return end();
}


FrameReference TrackingDataDirectory::FrameReferenceAt(FrameID frameID) const {
	auto fi = d_referencesByFID->find(frameID);
	if ( fi != d_referencesByFID->cend() ) {
		return fi->second;
	}
	return (*FrameAt(frameID))->Frame();
}

FrameReference TrackingDataDirectory::FrameReferenceAfter(const Time & t) const {
	auto fi = d_frameIDByTime.find(t.SortKey());
	if ( fi != d_frameIDByTime.cend() ) {
		return FrameReferenceAt(fi->second);
	}
	return (*FrameAfter(t))->Frame();
}


const TrackingDataDirectory::MovieIndex & TrackingDataDirectory::MovieSegments() const {
	return *d_movies;
}

TrackingDataDirectory::Ptr TrackingDataDirectory::Itself() const  {
	if ( auto locked = d_itself.lock() ) {
		return locked;
	}
	throw DeletedReference<TrackingDataDirectory>();
}

const TrackingDataDirectory::FrameReferenceCache &
TrackingDataDirectory::ReferenceCache() const {
	return *d_referencesByFID;
}


TrackingDataDirectory::Ptr
TrackingDataDirectory::LoadFromCache(const fs::path & absoluteFilePath,
                                     const std::string & URI) {
	return proto::TDDCache::Load(absoluteFilePath,URI);
}

void TrackingDataDirectory::SaveToCache() const {
	proto::TDDCache::Save(Itself());
}

std::shared_ptr<std::map<FrameReference,fs::path>>
TrackingDataDirectory::EnumerateFullFrames(const fs::path & subpath) const noexcept {
	auto dirpath = AbsoluteFilePath() / subpath;
	if (fs::is_directory(dirpath) == false ) {
		return {};
	}

	try {
		auto listing = ListTagCloseUpFiles(dirpath);
		auto res = std::make_shared<std::map<FrameReference,fs::path>>();
		for(const auto & [frameID,fileAndFilter] : listing) {
			if ( !fileAndFilter.second == true ) {
				res->insert(std::make_pair(FrameReferenceAt(frameID),fileAndFilter.first));
			}
		}
		return res;
	} catch (const std::exception & e ) {
	}
	return {};
}


TrackingDataDirectory::ComputedRessourceUnavailable::ComputedRessourceUnavailable(const std::string & typeName) noexcept
	: std::runtime_error("Computed ressource "+ typeName + " is not available") {
}
TrackingDataDirectory::ComputedRessourceUnavailable::~ComputedRessourceUnavailable() noexcept {
}


const std::vector<TagCloseUp::ConstPtr> &
TrackingDataDirectory::TagCloseUps() const {
	if ( TagCloseUpsComputed() == false ) {
		throw ComputedRessourceUnavailable("TagCloseUp");
	}
	return *d_tagCloseUps;
}


const std::map<FrameReference,fs::path> &
TrackingDataDirectory::FullFrames() const {
	if ( FullFramesComputed() == false ) {
		throw ComputedRessourceUnavailable("FullFrame");
	}
	return *d_fullFrames;
}

const TagStatisticsHelper::Timed &
TrackingDataDirectory::TagStatistics() const {
	if ( TagStatisticsComputed() == false ) {
		throw ComputedRessourceUnavailable("TagStatistics");
	}
	return *d_tagStatistics;
}


bool TrackingDataDirectory::TagCloseUpsComputed() const {
	return !d_tagCloseUps == false;
}

bool TrackingDataDirectory::TagStatisticsComputed() const {
	return !d_tagStatistics == false;
}

bool TrackingDataDirectory::FullFramesComputed() const {
	return !d_fullFrames == false;
}

class TagCloseUpsReducer {
public:
	TagCloseUpsReducer(size_t count,
	                   const TrackingDataDirectory::Ptr & tdd)
		: d_tdd(tdd)
		, d_closeUps(count) {
		d_count.store(count);
	}

	void Compute(size_t index,
	             FrameID frameID,
	             const TrackingDataDirectory::TagCloseUpFileAndFilter & fileAndFilter) {
		auto detector = d_detectorPool.Get(d_tdd->DetectionSettings());

		auto tcus = detector->Detect(fileAndFilter,
		                             d_tdd->FrameReferenceAt(frameID));

		Reduce(index,tcus);
	}

	void Reduce(size_t index,
	            const std::vector<TagCloseUp::ConstPtr> & tcus) {
		d_closeUps[index] = tcus;
		if ( (d_count.fetch_sub(1) - 1 ) > 0 ) {
			return;
		}
		d_tdd->d_tagCloseUps = std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
		for( const auto tcus : d_closeUps ) {
			d_tdd->d_tagCloseUps->insert(d_tdd->d_tagCloseUps->end(),
			                             tcus.begin(),
			                             tcus.end());
		}
		proto::TagCloseUpCache::Save(d_tdd->AbsoluteFilePath(),
		                             *d_tdd->d_tagCloseUps);
	}

private:
	class Detector {
	public:
		Detector(const tags::ApriltagOptions & detectorOptions) {
			const auto & [constructor,destructor] = tags::GetFamily(detectorOptions.Family);
			d_family = constructor();
			d_destructor = destructor;
			d_detector = apriltag_detector_create();

			detectorOptions.SetUpDetector(d_detector);
			apriltag_detector_add_family(d_detector,d_family);
		}

		~Detector() {
			apriltag_detector_destroy(d_detector);
			d_destructor(d_family);
		}

		std::vector<TagCloseUp::ConstPtr> Detect(const TrackingDataDirectory::TagCloseUpFileAndFilter & fileAndFilter,
		                                         const FrameReference & reference) {
			std::vector<TagCloseUp::ConstPtr> res;
			cv::Mat imgCv;

			try {
				imgCv = cv::imread(fileAndFilter.first.string(),cv::IMREAD_GRAYSCALE);
			} catch ( const std::exception	&  ) {
				return res;
			}

			if ( imgCv.empty() ) {
				return res;
			}

			image_u8_t img = {.width = imgCv.cols,.height = imgCv.rows, .stride = imgCv.cols, .buf = imgCv.data};
			zarray_t * detections = apriltag_detector_detect(d_detector,&img);
			Defer destroyDetections([detections]() { apriltag_detections_destroy(detections);});
			apriltag_detection * d;
			for ( size_t i = 0; i < zarray_size(detections); ++i) {
				zarray_get(detections,i,&d);
				if ( fileAndFilter.second && d->id != *fileAndFilter.second ) {
					continue;
				}
				res.push_back(std::make_shared<TagCloseUp>(fileAndFilter.first,reference,d));
			}
			return res;
		}

	private:
		apriltag_family_t    * d_family;
		tags::FamilyDestructor d_destructor;
		apriltag_detector_t  * d_detector;
	};

	std::atomic<size_t>                            d_count;
	TrackingDataDirectory::Ptr                     d_tdd;
	std::vector<std::vector<TagCloseUp::ConstPtr>> d_closeUps;
	utils::ObjectPool<Detector>                    d_detectorPool;
};


std::vector<TrackingDataDirectory::Loader>
TrackingDataDirectory::PrepareTagCloseUpsLoaders() {
	auto tagCloseUpFiles = ListTagCloseUpFiles(AbsoluteFilePath() / "ants");

	if ( tagCloseUpFiles.empty() || d_detectionSettings.Family == tags::Family::Undefined ) {
		d_tagCloseUps = std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
		proto::TagCloseUpCache::Save(AbsoluteFilePath(),{});
		return {};
	}

	auto reducer = std::make_shared<TagCloseUpsReducer>(tagCloseUpFiles.size(),
	                                                    Itself());
	size_t i = 0;
	std::vector<Loader> res;
	res.reserve(tagCloseUpFiles.size());
	for( const auto & [frameID,fileAndFilter] : tagCloseUpFiles ) {
		res.push_back([frameID,fileAndFilter,reducer,i]() {
			              reducer->Compute(i,frameID,fileAndFilter);
		              });
		++i;
	}

	return res;
}


class TagStatisticsReducer {
public:
	TagStatisticsReducer(size_t count,
	                     const TrackingDataDirectory::Ptr & tdd)
		: d_tdd(tdd)
		, d_stats(count){
		d_count.store(count);
	}

	void Reduce(size_t index,
	            const TagStatisticsHelper::Timed & stats) {
		d_stats[index] = stats;
		if ( (d_count.fetch_sub(1) - 1 ) > 0 ) {
			return;
		}
		d_tdd->d_tagStatistics = std::make_shared<TagStatisticsHelper::Timed>(TagStatisticsHelper::MergeTimed(d_stats.begin(),d_stats.end()));
		proto::TagStatisticsCache::Save(d_tdd->AbsoluteFilePath(),*d_tdd->d_tagStatistics);
	}
private:
	std::atomic<size_t> d_count;
	TrackingDataDirectory::Ptr d_tdd;
	std::vector<TagStatisticsHelper::Timed> d_stats;
};

std::vector<TrackingDataDirectory::Loader>
TrackingDataDirectory::PrepareTagStatisticsLoaders() {
	const auto & segments = d_segments->Segments();
	auto reducer = std::make_shared<TagStatisticsReducer>(segments.size(),
	                                                      Itself());

	std::vector<Loader> res;
	res.reserve(segments.size());
	size_t i = 0;
	for ( const auto & s : segments ) {

		res.push_back([reducer,s,i,this]() {
			              auto stats = TagStatisticsHelper::BuildStats((AbsoluteFilePath()/ s.second).string());
			              reducer->Reduce(i,stats);
		              });
		++i;
	}
	return res;
}


class FullFramesReducer {
public :
	FullFramesReducer(size_t count,
	                  const TrackingDataDirectory::Ptr & tdd)
		: d_tdd(tdd) {
		d_count.store(count);
	}

	void Reduce() {
		if ( (d_count.fetch_sub(1) - 1) > 0 ) {
			return;
		}
		d_tdd->d_fullFrames = d_tdd->EnumerateFullFrames("ants/computed");
	}
private:
	std::atomic<size_t>        d_count;
	TrackingDataDirectory::Ptr d_tdd;
};



std::vector<TrackingDataDirectory::Loader>
TrackingDataDirectory::PrepareFullFramesLoaders() {
	auto firstFrame = *begin();
	int width = firstFrame->Width();
	int height = firstFrame->Height();
	fs::create_directory(AbsoluteFilePath() / "ants/computed");
	auto reducer = std::make_shared<FullFramesReducer>(d_movies->Segments().size(),
	                                                   Itself());
	std::vector<Loader> res;
	for ( const auto & ms : d_movies->Segments() ) {
		res.push_back([reducer,ms,width,height,this]() {
			              cv::VideoCapture capture(ms.second->AbsoluteFilePath().c_str());
			              cv::Mat frame,scaled;
			              capture >> frame;
			              cv::resize(frame,scaled,cv::Size(width,height),cv::INTER_CUBIC);
			              auto filename = "frame_" + std::to_string(ms.second->ToTrackingFrameID(0)) + ".png";
			              auto imgPath = AbsoluteFilePath() / "ants/computed" / filename;
			              cv::imwrite(imgPath.c_str(),scaled);
			              reducer->Reduce();
		              });
	}

	return res;
}

void TrackingDataDirectory::LoadComputedFromCache() {
	try {
		d_tagStatistics = std::make_shared<TagStatisticsHelper::Timed>(proto::TagStatisticsCache::Load(AbsoluteFilePath()));
	} catch (const std::exception & e) {}

	try {
		d_tagCloseUps = std::make_shared<std::vector<TagCloseUp::ConstPtr>>();
		*d_tagCloseUps = proto::TagCloseUpCache::Load(AbsoluteFilePath(),
		                                              [this](FrameID frameID) -> FrameReference {
			                                              return FrameReferenceAt(frameID);
		                                              });
	} catch (const std::exception & e) {
		d_tagCloseUps.reset();
	}



	d_fullFrames = EnumerateFullFrames("ants");
	if ( !d_fullFrames || d_fullFrames->empty() ) {
		d_fullFrames = EnumerateFullFrames("ants/computed");
	}

}


void TrackingDataDirectory::LoadDetectionSettings() {
	auto letoConfig = YAML::LoadFile((AbsoluteFilePath() / "leto-final-config.yml").string());
	auto apriltagSettings = letoConfig["apriltag"];
	if (!apriltagSettings) {
		return;
	}
	if ( apriltagSettings["family"] ) {
		d_detectionSettings.Family = tags::FindFamily(apriltagSettings["family"].as<std::string>());
	}
	auto quadSettings = apriltagSettings["quad"];
	if ( !quadSettings ) {
		return;
	}
#define SET_IF_EXISTS(cppType,cppName,yamlName) do {	  \
		if ( quadSettings[#yamlName] ) { \
			d_detectionSettings.cppName = quadSettings[#yamlName].as<cppType>(); \
		} \
	}while(0)

	SET_IF_EXISTS(float,QuadDecimate,decimate);
	SET_IF_EXISTS(float,QuadSigma,sigma);
	SET_IF_EXISTS(bool,RefineEdges,refine-edges);
	SET_IF_EXISTS(int,QuadMinClusterPixel,min-cluster-pixel);
	SET_IF_EXISTS(int,QuadMaxNMaxima,max-n-maxima);
	SET_IF_EXISTS(float,QuadCriticalRadian,critical-angle-radian);
	SET_IF_EXISTS(float,QuadMaxLineMSE,max-line-mean-square-error);
	SET_IF_EXISTS(int,QuadMinBWDiff,min-black-white-diff);
	SET_IF_EXISTS(bool,QuadDeglitch,deglitch);

#undef SET_IF_EXISTS


}



}
}
}


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::TrackingDataDirectory & a) {
	return out << "TDD{URI:'" << a.URI()
	           << "', start:" << a.StartDate()
	           << ", end:" << a.EndDate()
	           << "}";
}
