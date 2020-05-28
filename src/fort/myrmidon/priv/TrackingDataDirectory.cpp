#include "TrackingDataDirectory.hpp"

#include "RawFrame.hpp"

#include <mutex>
#include <tbb/parallel_for.h>

#include <fort/hermes/Error.h>
#include <fort/hermes/FileContext.h>

#include <fort/myrmidon/utils/NotYetImplemented.hpp>
#include <fort/myrmidon/utils/Checker.hpp>
#include "TagCloseUp.hpp"

#include "TimeUtils.hpp"

#include <fort/myrmidon/priv/proto/TDDCache.hpp>

#include <fort/myrmidon/utils/Defer.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#ifdef MYRMIDON_USE_BOOST_FILESYSTEM
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::regular_file)
#else
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::file_type::regular)
#endif


namespace fort {
namespace myrmidon {
namespace priv {

TrackingDataDirectory::ConstPtr TrackingDataDirectory::Create(const std::string & uri,
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
	, d_endIterator(ConstPtr(),endFrame+1)
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
		std::cerr << "Looking for " << f.path().generic_string() << std::endl;
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
                                              MovieSegment::List & movies ) {
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

void TrackingDataDirectory::BuildCache(const std::string & URI,
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
					FrameID curFID = ro.frameid();
					Time curTime = TimeFromFrameReadout(ro,monoID);
					if ( *iter == curFID ) {
						Times.push_back(curTime);
						++iter;
					}
				} catch ( const fort::hermes::EndOfFile & ) {

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
		toFind[file].ToFind.insert(ref.FID());
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

			FrameID curFID = ro.frameid();
			FrameReference curReference(URI,curFID,startTime);
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

TrackingDataDirectory::ConstPtr TrackingDataDirectory::Open(const fs::path & filepath, const fs::path & experimentRoot) {
	CheckPaths(filepath,experimentRoot);

	auto absoluteFilePath = fs::weakly_canonical(fs::absolute(filepath));
	auto URI = fs::relative(absoluteFilePath,fs::absolute(experimentRoot));

	try {
		return LoadFromCache(absoluteFilePath,URI.generic_string());
	} catch (const std::exception & e ) {
	};


	std::vector<fs::path> hermesFiles;
	std::map<uint32_t,std::pair<fs::path,fs::path> > moviesPaths;
	MovieSegment::List movies;
	auto ti = std::make_shared<TrackingIndex>();
	auto mi = std::make_shared<MovieIndex>();
	auto referenceCache = std::make_shared<FrameReferenceCache>();

	LookUpFiles(absoluteFilePath,hermesFiles,moviesPaths);
	if ( hermesFiles.empty() ) {
		throw std::invalid_argument(filepath.string() + " does not contains any .hermes file");
	}
	LoadMovieSegments(moviesPaths,URI.generic_string(),movies);
	for(const auto & m : movies) {
		referenceCache->insert(std::make_pair(m->StartFrame(),FrameReference(URI.generic_string(),0,Time())));
		referenceCache->insert(std::make_pair(m->EndFrame(),FrameReference(URI.generic_string(),0,Time())));
	}

	auto snapshots = TagCloseUp::Lister::ListFiles(absoluteFilePath / "ants");
	for(const auto & [FID,s] : snapshots) {
		referenceCache->insert(std::make_pair(FID,FrameReference(URI.generic_string(),0,Time())));
	}

	Time::MonoclockID monoID = GetUID(absoluteFilePath);

	auto bounds = BuildIndexes(URI.generic_string(),
	                           monoID,
	                           hermesFiles,
	                           ti);

	BuildCache(URI.generic_string(),
	           monoID,
	           absoluteFilePath,
	           ti,
	           *referenceCache);
	// caches the last frame
	referenceCache->insert(std::make_pair(bounds.second.first,
	                                      FrameReference(URI.generic_string(),
	                                                     bounds.second.first,
	                                                     bounds.second.second.Add(-1))));
	Time emptyTime;

	for(const auto & m : movies) {
		auto fi = referenceCache->find(m->StartFrame());
		if (fi == referenceCache->cend() ||
		    ( fi->second.FID() == 0 && fi->second.Time().Equals(emptyTime) ) ) {
			std::ostringstream oss;
			oss << "[MovieIndexing] Could not find FrameReference for FrameID " << m->StartFrame();
			throw std::logic_error(oss.str());
		}
		mi->Insert(fi->second,m);
	}

	std::vector<FrameID> toErase;
	toErase.reserve(referenceCache->size());
	for ( const auto & [FID,ref] : *referenceCache ) {
		if (ref.FID() == 0 && ref.Time().Equals(emptyTime) ) {
			toErase.push_back(FID);
		}
	}

	for( auto FID : toErase ) {
		std::cerr << "[CacheCleaning] Could not find FrameReference for FrameID " << FID << std::endl;
		referenceCache->erase(FID);
	}


	auto res = TrackingDataDirectory::Create(URI.generic_string(),
	                                         absoluteFilePath,
	                                         bounds.first.first,
	                                         bounds.second.first,
	                                         bounds.first.second,
	                                         bounds.second.second,
	                                         ti,
	                                         mi,
	                                         referenceCache);
	try {
		res->SaveToCache();
	} catch(const std::exception & e) {
	}

	return res;
}


const TrackingDataDirectory::TrackingIndex &
TrackingDataDirectory::TrackingSegments() const {
	return *d_segments;
}


TrackingDataDirectory::const_iterator::const_iterator(const TrackingDataDirectory::ConstPtr & parent,
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
	while ( !d_frame || d_frame->Frame().FID() < d_current) {
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
	if ( d_frame->Frame().FID() > d_current ) {
		d_current = d_frame->Frame().FID();
	}
	return d_frame;
}


TrackingDataDirectory::ConstPtr TrackingDataDirectory::const_iterator::LockParent() const {
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
	auto iter = FrameAt(d_segments->Find(t).first.FID());
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


FrameReference TrackingDataDirectory::FrameReferenceAt(FrameID FID) const {
	auto fi = d_referencesByFID->find(FID);
	if ( fi != d_referencesByFID->cend() ) {
		return fi->second;
	}
	return (*FrameAt(FID))->Frame();
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

TrackingDataDirectory::ConstPtr TrackingDataDirectory::Itself() const  {
	if ( auto locked = d_itself.lock() ) {
		return locked;
	}
	throw DeletedReference<TrackingDataDirectory>();
}

const TrackingDataDirectory::FrameReferenceCache &
TrackingDataDirectory::ReferenceCache() const {
	return *d_referencesByFID;
}


TrackingDataDirectory::ConstPtr
TrackingDataDirectory::LoadFromCache(const fs::path & absoluteFilePath,
                                     const std::string & URI) {
	return proto::TDDCache::Load(absoluteFilePath,URI);
}

void TrackingDataDirectory::SaveToCache() const {
	proto::TDDCache::Save(Itself());
}

const TagCloseUp::Lister::Ptr
TrackingDataDirectory::TagCloseUpLister(tags::Family f,
                                        uint8_t threshold) const {
	PERF_FUNCTION();

	auto locked = Itself();
	return TagCloseUp::Lister::Create(d_absoluteFilePath / "ants",
	                                  f,
	                                  threshold,
	                                  [locked](FrameID fid) {
		                                  return locked->FrameReferenceAt(fid);
	                                  });
}


std::map<FrameReference,fs::path>
TrackingDataDirectory::FullFramesFor(const fs::path & subpath) const {
	std::map<FrameReference,fs::path> res;

	auto listing = TagCloseUp::Lister::ListFiles(AbsoluteFilePath() / subpath);
	for(const auto & [FID,fileAndFilter] : listing) {
		if ( !fileAndFilter.second == true ) {
			res.insert(std::make_pair(FrameReferenceAt(FID),fileAndFilter.first));
		}
	}

	return res;
}

void TrackingDataDirectory::ComputeFullFrames() const {
	auto firstFrame = *begin();
	int width = firstFrame->Width();
	int height = firstFrame->Height();
	const auto & ms = d_movies->Segments();
	fs::create_directory(AbsoluteFilePath() / "ants/computed");
	tbb::parallel_for(tbb::blocked_range<size_t>(0,ms.size()),
	                  [this,
	                   &ms,
	                   width,
	                   height](const tbb::blocked_range<size_t> & range) {
		                  for ( size_t i = range.begin();
		                        i != range.end();
		                        ++i ) {
			                  cv::VideoCapture capture(ms[i].second->AbsoluteFilePath().c_str());
			                  cv::Mat frame,scaled;
			                  capture >> frame;
			                  cv::resize(frame,scaled,cv::Size(width,height),cv::INTER_CUBIC);
			                  auto filename = "frame_" + std::to_string(ms[i].second->ToTrackingFrameID(0)) + ".png";
			                  auto imgPath = AbsoluteFilePath() / "ants/computed" / filename;
			                  cv::imwrite(imgPath.c_str(),scaled);
		                  }
	                  });



}

std::map<FrameReference,fs::path>
TrackingDataDirectory::FullFrames() const {
	return FullFramesFor("ants");
}

std::map<FrameReference,fs::path>
TrackingDataDirectory::ComputedFullFrames() const {
	if ( fs::is_directory(AbsoluteFilePath() / "ants/computed") == false ) {
		ComputeFullFrames();
	}
	return FullFramesFor("ants/computed");
}


std::vector<TagStatisticsHelper::Loader>
TrackingDataDirectory::StatisticsLoader() const {
	std::vector<TagStatisticsHelper::Loader> res;
	res.reserve(d_segments->Segments().size());
	for ( const auto & [ref,segment] : d_segments->Segments() ) {
		std::string filepath = (AbsoluteFilePath() / segment).string();
		res.push_back([filepath] () { return TagStatisticsHelper::BuildStats(filepath); });
	}
	return res;
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
