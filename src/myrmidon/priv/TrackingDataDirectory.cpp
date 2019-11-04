#include "TrackingDataDirectory.hpp"

#include <mutex>

#include <fort-hermes/Error.h>
#include <fort-hermes/FileContext.h>



namespace fort {
namespace myrmidon {
namespace priv {

TrackingDataDirectory::TrackingDataDirectory()
	: d_startFrame(0)
	, d_endFrame(0) {
}

TrackingDataDirectory::TrackingDataDirectory(const fs::path & path,
                                             uint64_t startFrame,
                                             uint64_t endFrame,
                                             const Time & startdate,
                                             const Time & enddate,
                                             const SegmentIndexer & si)
	: d_path(path)
	, d_startFrame(startFrame)
	, d_endFrame(endFrame)
	, d_startDate(startdate)
	, d_endDate(enddate)
	, d_segments(si) {
	if ( d_startFrame >= d_endFrame ) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startFrame:" << d_startFrame << " >= endDate: " << d_endFrame;
		throw std::invalid_argument(os.str());
	}

	if ( d_startDate.Before(d_endDate) == false ) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startDate:" << d_startDate << " >= endDate: " << d_endDate;
		throw std::invalid_argument(os.str());
	}
}


const fs::path &  TrackingDataDirectory::Path() const {
	return d_path;
}

uint64_t TrackingDataDirectory::StartFrame() const {
	return d_startFrame;
}

uint64_t TrackingDataDirectory::EndFrame() const {
	return d_endFrame;
}

const Time & TrackingDataDirectory::StartDate() const {
	return d_startDate;
}

const Time & TrackingDataDirectory::EndDate() const {
	return d_endDate;
}

TrackingDataDirectory::UID TrackingDataDirectory::GetUID(const fs::path & path,  const fs::path & base) {
	static std::mutex mutex;
	static UID last = 0;
	static std::map<fs::path,UID> d_UIDs;
	std::lock_guard<std::mutex> lock(mutex);
	fs::path fpath = base / path;
	auto fi = d_UIDs.find(fpath);
	if ( fi == d_UIDs.end() ) {
		d_UIDs.insert(std::make_pair(fpath,++last));
		return last;
	}
	return fi->second;
}



TrackingDataDirectory TrackingDataDirectory::Open(const fs::path & path, const fs::path & base) {
	if ( fs::is_directory(base) == false ) {
		throw std::invalid_argument("base path " + base.string() +  " is not a directory");
	}
	if ( fs::is_directory(path) == false ) {
		throw std::invalid_argument( path.string() + " is not a directory");
	}


	std::vector<fs::path> hermesFiles;

	for( auto const & f : fs::directory_iterator(path) ) {
#ifdef MYRMIDON_USE_BOOST_FILESYSTEM
		if ( f.status().type() != fs::regular_file ) {
#else
		if ( f.status().type() != fs::file_type::regular ) {
#endif
			continue;
		}

		if ( f.path().extension() != ".hermes") {
			continue;
		}
		hermesFiles.push_back(f.path());
	}
	if ( hermesFiles.empty() ) {
		throw std::invalid_argument(path.string() + " does not contains any .hermes file");
	}

	Time::MonoclockID monoID = TrackingDataDirectory::GetUID(path,base);


	uint64_t start,end;
	Time startDate,endDate;

	std::sort(hermesFiles.begin(),hermesFiles.end());

	fort::hermes::FrameReadout ro;
	SegmentIndexer si;
	bool first = true;
	std::shared_ptr<fort::hermes::FileContext> fc;

	for (const auto & f : hermesFiles ) {
		try {
			fc = std::make_shared<fort::hermes::FileContext>(f.string());
			fc->Read(&ro);
			Time startTime = Time::FromTimestampAndMonotonic(ro.time(),ro.timestamp(),monoID);

			if ( first == true) {
				start = ro.frameid();
				startDate = startTime;
				first = false;
			}
			si.Insert(ro.frameid(),startTime,f.generic_string());
		} catch ( const std::exception & e) {
			throw std::runtime_error("Could not extract frame from " +  f.string() + ": " + e.what());
		}
	}
	try {
		for (;;) {
			fc->Read(&ro);
			end = ro.frameid();
			endDate = Time::FromTimestampAndMonotonic(ro.time(),ro.timestamp(),monoID);
		}
	} catch ( const fort::hermes::EndOfFile &) {
		//DO nothing, we just reached EOF
	} catch ( const std::exception & e) {
		throw std::runtime_error("Could not extract last frame from " +  hermesFiles.back().string() + ": " + e.what());
	}


	return TrackingDataDirectory(fs::relative(path,base),
	                             start,
	                             end,
	                             startDate,
	                             endDate,
	                             si);
	}


const SegmentIndexer & TrackingDataDirectory::TrackingIndex() const {
	return d_segments;
}

}
}
}
