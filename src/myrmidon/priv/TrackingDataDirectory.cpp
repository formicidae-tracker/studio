#include "TrackingDataDirectory.hpp"

#include "RawFrame.hpp"

#include <mutex>

#include <fort-hermes/Error.h>
#include <fort-hermes/FileContext.h>

#include "../utils/NotYetImplemented.hpp"


#ifdef MYRMIDON_USE_BOOST_FILESYSTEM
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::regular_file)
#else
#define MYRMIDON_FILE_IS_REGULAR(f) ((f).type() == fs::file_type::regular)
#endif


namespace fort {
namespace myrmidon {
namespace priv {

TrackingDataDirectory::TrackingDataDirectory()
	: d_startFrame(0)
	, d_endFrame(0)
	, d_uid(0)
	, d_endIterator("",SegmentIndexer::Ptr(),0,0,1,0) {
}

TrackingDataDirectory::TrackingDataDirectory(const fs::path & path,
                                             const fs::path & experimentRoot,
                                             uint64_t startFrame,
                                             uint64_t endFrame,
                                             const Time & startdate,
                                             const Time & enddate,
                                             const SegmentIndexer::Ptr & si,
                                             const MovieSegment::List & movies)
	: d_experimentRoot(fs::weakly_canonical(experimentRoot))
	, d_path(fs::relative(path,d_experimentRoot))
	, d_startFrame(startFrame)
	, d_endFrame(endFrame)
	, d_segments(si)
	, d_movies(movies)
	, d_uid(GetUID(d_experimentRoot/d_path))
	, d_endIterator(d_experimentRoot/d_path,si,startFrame,endFrame,endFrame+1,d_uid) {

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


const fs::path &  TrackingDataDirectory::LocalPath() const {
	return d_path;
}

fs::path  TrackingDataDirectory::FilePath() const {
	return d_experimentRoot / d_path;
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
	fs::path fpath = fs::weakly_canonical(filepath);
	auto fi = d_UIDs.find(fpath);
	if ( fi == d_UIDs.end() ) {
		d_UIDs.insert(std::make_pair(fpath,++last));
		return last;
	}
	return fi->second;
}


TrackingDataDirectory TrackingDataDirectory::Open(const fs::path & path, const fs::path & experimentRoot) {
	if ( fs::is_directory(experimentRoot) == false ) {
		throw std::invalid_argument("experiment root path " + experimentRoot.string() +  " is not a directory");
	}
	if ( fs::is_directory(path) == false ) {
		throw std::invalid_argument( path.string() + " is not a directory");
	}


	std::vector<fs::path> hermesFiles;
	std::map<uint32_t,std::pair<fs::path,fs::path> > moviesPaths;
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
	for( auto const & f : fs::directory_iterator(path) ) {
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

	if ( hermesFiles.empty() ) {
		throw std::invalid_argument(path.string() + " does not contains any .hermes file");
	}

	Time::MonoclockID monoID = TrackingDataDirectory::GetUID(path);


	uint64_t start,end;
	Time startDate,endDate;

	std::sort(hermesFiles.begin(),hermesFiles.end());

	fort::hermes::FrameReadout ro;
	auto si = std::make_shared<SegmentIndexer>();
	bool first = true;
	std::shared_ptr<fort::hermes::FileContext> fc;

	for (const auto & f : hermesFiles ) {
		try {
			fc = std::make_shared<fort::hermes::FileContext>(f.string());
			fc->Read(&ro);
			Time startTime = Time::FromTimestampAndMonotonic(ro.time(),ro.timestamp()*1000,monoID);

			if ( first == true) {
				start = ro.frameid();
				startDate = startTime;
				first = false;
			}
			si->Insert(ro.frameid(),startTime,fs::relative(f,path).generic_string());
		} catch ( const std::exception & e) {
			throw std::runtime_error("Could not extract frame from " +  f.string() + ": " + e.what());
		}
	}
	try {
		for (;;) {
			fc->Read(&ro);
			end = ro.frameid();
			//we add 1 nanosecond to transform the valid range from
			//[start;end[ to [start;end] by making it
			//[start;end+1ns[. There are no time existing between end
			//and end+1ns;
			endDate = Time::FromTimestampAndMonotonic(ro.time(),ro.timestamp()*1000,monoID).Add(1);
		}
	} catch ( const fort::hermes::EndOfFile &) {
		//DO nothing, we just reached EOF
	} catch ( const std::exception & e) {
		throw std::runtime_error("Could not extract last frame from " +  hermesFiles.back().string() + ": " + e.what());
	}

	MovieSegment::List movies;
	for ( const auto & [id,paths] : moviesPaths ) {
		movies.push_back(MovieSegment::Open(paths.first,paths.second));
	}

	return TrackingDataDirectory(path,
	                             experimentRoot,
	                             start,
	                             end,
	                             startDate,
	                             endDate,
	                             si,
	                             movies);
}


const SegmentIndexer & TrackingDataDirectory::TrackingIndex() const {
	return *d_segments;
}


TrackingDataDirectory::const_iterator::const_iterator(const fs::path & parentPath,
                                                      const SegmentIndexer::ConstPtr & segments,
                                                      uint64_t start,
                                                      uint64_t end,
                                                      uint64_t current,
                                                      TrackingDataDirectory::UID uid)
	: d_parentPath(parentPath)
	, d_segments(segments)
	, d_start(start)
	, d_end(end)
	, d_current(current)
	, d_uid(uid) {
}

TrackingDataDirectory::const_iterator::const_iterator(const const_iterator & other)
	: d_parentPath(other.d_parentPath)
	, d_segments(other.d_segments)
	, d_start(other.d_start)
	, d_end(other.d_end)
	, d_current(other.d_current)
	, d_uid(other.d_uid) {
}

TrackingDataDirectory::const_iterator& TrackingDataDirectory::const_iterator::operator++() {
	if ( d_current <= d_end ) {
		++d_current;
	}
	return *this;
}

bool TrackingDataDirectory::const_iterator::operator==(const const_iterator & other) const {
	return (d_uid == other.d_uid) && (d_current == other.d_current);
}

bool TrackingDataDirectory::const_iterator::operator!=(const const_iterator & other) const {
	return !(*this == other);
}

const RawFrameConstPtr TrackingDataDirectory::const_iterator::NULLPTR;

const RawFrameConstPtr & TrackingDataDirectory::const_iterator::operator*() {
	if ( d_current > d_end ) {
		return NULLPTR;
	}
	while ( !d_frame || d_frame->ID() < d_current) {
		if ( !d_file ) {
			auto p = d_parentPath / d_segments->Find(d_current);
			d_file = std::unique_ptr<fort::hermes::FileContext>(new fort::hermes::FileContext(p.string()));
			d_message.Clear();
		}

		try {
			d_file->Read(&d_message);
			d_frame = RawFrame::Create(d_parentPath.generic_string(),d_message,d_uid);

		} catch( const fort::hermes::EndOfFile & ) {
			d_current = d_end + 1;
			d_frame.reset();
			return NULLPTR;
		}
	}
	if ( d_frame->ID() > d_current ) {
		d_current = d_frame->ID();
	}
	return d_frame;
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::begin() const {
	return const_iterator(d_experimentRoot / d_path,d_segments,d_startFrame,d_endFrame,d_startFrame,GetUID());
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameAt(uint64_t frameID) const {
	if ( frameID < d_startFrame || frameID > d_endFrame ) {
		std::ostringstream os;
		os << "Could not find frame " << frameID << " in [" << d_startFrame << ";" << d_endFrame << "]";
		throw std::out_of_range(os.str());
	}
	return const_iterator(d_experimentRoot / d_path,d_segments,d_startFrame,d_endFrame,frameID,GetUID());
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameNear(const Time & t) const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}


const MovieSegment::List & TrackingDataDirectory::MovieSegments() const {
	return d_movies;
}



}
}
}
