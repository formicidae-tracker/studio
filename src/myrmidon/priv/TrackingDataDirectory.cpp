#include "TrackingDataDirectory.hpp"

#include "RawFrame.hpp"

#include <mutex>

#include <fort-hermes/Error.h>
#include <fort-hermes/FileContext.h>

#include "../utils/NotYetImplemented.hpp"
#include "SnapshotCache.hpp"

#include "TimeUtils.hpp"

#ifdef MYRMIDON_USE_BOOST_FILESYSTEM
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::regular_file)
#else
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::file_type::regular)
#endif


namespace fort {
namespace myrmidon {
namespace priv {

TrackingDataDirectory::ConstPtr TrackingDataDirectory::Create(const fs::path & uri,
                                                              const fs::path & absoluteFilePath,
                                                              uint64_t startFrame,
                                                              uint64_t endFrame,
                                                              const Time & startdate,
                                                              const Time & enddate,
                                                              const TrackingIndex::Ptr & si,
                                                              const MovieIndex::Ptr & movies,
                                                              const FrameReferenceCache & referenceCache) {
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


TrackingDataDirectory::TrackingDataDirectory(const fs::path & uri,
                                             const fs::path & absoluteFilePath,
                                             uint64_t startFrame,
                                             uint64_t endFrame,
                                             const Time & startdate,
                                             const Time & enddate,
                                             const TrackingIndex::Ptr & si,
                                             const MovieIndex::Ptr & movies,
                                             const FrameReferenceCache & referenceCache)
	: d_absoluteFilePath(absoluteFilePath)
	, d_URI(uri)
	, d_startFrame(startFrame)
	, d_endFrame(endFrame)
	, d_uid(GetUID(d_absoluteFilePath))
	, d_endIterator(ConstPtr(),endFrame)
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

}


const fs::path &  TrackingDataDirectory::URI() const {
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
                                              MovieSegment::List & movies ){
	for ( const auto & [id,paths] : moviesPaths ) {
		if ( !paths.first.empty() && !paths.second.empty() ) {
			movies.push_back(MovieSegment::Open(paths.first,paths.second));
		}
	}

	std::sort(movies.begin(),movies.end(),[](const MovieSegment::Ptr & a,
	                                         const MovieSegment::Ptr & b) {
		                                      return a->StartFrame() < b->StartFrame();
	                                      });
}


std::pair<TrackingDataDirectory::TimedFrame,TrackingDataDirectory::TimedFrame>
TrackingDataDirectory::BuildIndexes(const fs::path & URI,
                                    Time::MonoclockID monoID,
                                    const std::vector<fs::path> & hermesFiles,
                                    const TrackingIndex::Ptr & trackingIndexer,
                                    FrameReferenceCache & cache) {

	uint64_t start,end;
	Time startDate,endDate;

	fort::hermes::FrameReadout ro,lastRo;
	bool first = true;
	std::shared_ptr<fort::hermes::FileContext> fc,prevFc;
	auto cacheIter = cache.begin();

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

			if (!prevFc) {
				prevFc = fc;
				lastRo = ro;
				continue;
			}

			bool ok = true;
			for(; ok == true && cacheIter != cache.end() && cacheIter->first < curFID; ++cacheIter ) {
				while (lastRo.frameid() < cacheIter->first) {
					try {
						prevFc->Read(&lastRo);
					} catch ( const std::exception & e ) {
						ok = false;
					}
				}

				Time curTime = TimeFromFrameReadout(lastRo,monoID);

				cacheIter->second = FrameReference(URI,
				                                   lastRo.frameid(),
				                                   curTime);
			}

			if ( cacheIter != cache.end() && cacheIter->first == curFID ) {
				cacheIter->second = curReference;
				++cacheIter;
			}

			prevFc = fc;
			lastRo = ro;

		} catch ( const std::exception & e) {
			throw std::runtime_error("Could not extract frame from " +  f.string() + ": " + e.what());
		}
	}
	try {
		for (;;) {
			fc->Read(&ro);
			end = ro.frameid();
			endDate = TimeFromFrameReadout(ro,monoID);

			if ( cacheIter != cache.end() && cacheIter->first == end ) {
				cacheIter->second = FrameReference(URI,end,endDate);
				++cacheIter;
			}

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

	try {
		return Load(filepath,experimentRoot);
	} catch (const std::exception & e ) {
	};

	auto absoluteFilePath = fs::weakly_canonical(fs::absolute(filepath));
	auto URI = fs::relative(absoluteFilePath,fs::absolute(experimentRoot));

	std::vector<fs::path> hermesFiles;
	std::map<uint32_t,std::pair<fs::path,fs::path> > moviesPaths;
	MovieSegment::List movies;
	auto ti = std::make_shared<TrackingIndex>();
	auto mi = std::make_shared<MovieIndex>();
	FrameReferenceCache referenceCache;

	LookUpFiles(absoluteFilePath,hermesFiles,moviesPaths);

	if ( hermesFiles.empty() ) {
		throw std::invalid_argument(filepath.string() + " does not contains any .hermes file");
	}

	LoadMovieSegments(moviesPaths,movies);
	for(const auto & m : movies) {
		referenceCache.insert(std::make_pair(m->StartFrame(),FrameReference(URI,0,Time())));
	}

	auto snapshots = SnapshotIndex::ListSnapshotFiles(absoluteFilePath / "ants");
	for(const auto & [FID,s] : snapshots) {
		referenceCache.insert(std::make_pair(FID,FrameReference(URI,0,Time())));
	}

	Time::MonoclockID monoID = GetUID(absoluteFilePath);

	auto bounds = BuildIndexes(absoluteFilePath,
	                           monoID,
	                           hermesFiles,
	                           ti,
	                           referenceCache);
	Time emptyTime;

	for(const auto & m : movies) {
		auto fi = referenceCache.find(m->StartFrame());
		if (fi == referenceCache.end() ||
		    ( fi->second.ID() == 0 && fi->second.Time().Equals(emptyTime) ) ) {
			std::ostringstream oss;
			oss << "Could not find FrameReference for FrameID " << m->StartFrame();
			throw std::logic_error(oss.str());
		}
		mi->Insert(fi->second,m);
	}

	std::vector<FrameID> toErase;
	toErase.reserve(referenceCache.size());
	for ( const auto & [FID,ref] : referenceCache ) {
		if (ref.ID() == 0 && ref.Time().Equals(emptyTime) ) {
			toErase.push_back(FID);
		}
	}

	for( auto FID : toErase ) {
		std::cerr << "Could not find FrameReference for FrameID " << FID << std::endl;
		referenceCache.erase(FID);
	}


	auto res = TrackingDataDirectory::Create(URI,
	                                         absoluteFilePath,
	                                         bounds.first.first,
	                                         bounds.second.first,
	                                         bounds.first.second,
	                                         bounds.second.second,
	                                         ti,
	                                         mi,
	                                         referenceCache);
	try {
		res->Save();
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

TrackingDataDirectory::const_iterator::const_iterator(const const_iterator & other)
	: d_parent(other.d_parent)
	, d_current(other.d_current) {
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
	while ( !d_frame || d_frame->ID() < d_current) {
		if ( !d_file ) {
			auto p = parent->d_absoluteFilePath / parent->d_segments->Find(d_current);
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
	if ( d_frame->ID() > d_current ) {
		d_current = d_frame->ID();
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

TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameNear(const Time & t) const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
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

TrackingDataDirectory::ConstPtr TrackingDataDirectory::Load(const fs::path &,const fs::path &) {
	throw std::runtime_error("foo");
}

void TrackingDataDirectory::Save() const {
	throw std::runtime_error("foo");
}

}
}
}
