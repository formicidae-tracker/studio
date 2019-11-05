#include "TrackingDataDirectory.hpp"

#include "RawFrame.hpp"

#include <mutex>

#include <fort-hermes/Error.h>
#include <fort-hermes/FileContext.h>

#include "../utils/NotYetImplemented.hpp"


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
	, d_segments(si) {
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
	return *d_start;
}

const Time & TrackingDataDirectory::EndDate() const {
	return *d_end;
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
			//we add 1 nanosecond to transform the valid range from
			//[start;end[ to [start;end] by making it
			//[start;end+1ns[. There are no time existing between end
			//and end+1ns;
			endDate = Time::FromTimestampAndMonotonic(ro.time(),ro.timestamp(),monoID).Add(1);
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

 TrackingDataDirectory::const_iterator::const_iterator() {}

TrackingDataDirectory::const_iterator::const_iterator(const fs::path & filepath,
                                                      const fs::path & parentPath,
                                                      TrackingDataDirectory::UID uid)
	: d_file(std::unique_ptr<hermes::FileContext>( new hermes::FileContext(filepath.string())))
	, d_parentPath(parentPath)
	, d_uid(uid) {
	++(*this);
}

TrackingDataDirectory::const_iterator& TrackingDataDirectory::const_iterator::operator++() {
	try {
		d_file->Read(&d_message);
		d_frame = RawFrame::Create(d_parentPath,d_message,d_uid);
	} catch ( fort::hermes::EndOfFile & ) {
		d_file.reset();
		d_frame.reset();
		d_message.Clear();
	}
}

bool TrackingDataDirectory::const_iterator::operator==(const const_iterator & other) const {
	if ( !d_frame ) {
		return !other.d_frame;
	}

	if (!other.d_frame) {
		return false;
	}

	return d_frame->Time().MonoID() == other.d_frame->Time().MonoID() &&
		d_frame->FrameID() == other.d_frame->FrameID();
}

bool TrackingDataDirectory::const_iterator::operator!=(const const_iterator & other) const {
	return !(*this == other);
}

RawFrameConstPtr TrackingDataDirectory::const_iterator::operator*() const{
	return d_frame;
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::begin() const {
	auto first = d_segments.Find(d_startFrame);
	return const_iterator(first,d_path,d_start->MonoID());
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::end() const {
	return const_iterator();
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameAt(uint64_t frameID) const {
	auto start = d_segments.Find(frameID);
	for(auto res = 	const_iterator(start,d_path,d_start->MonoID()) ;res != end() ; ++res) {
		if ((*res)->FrameID() == frameID ) {
			return res;
		}
		if ((*res)->FrameID() > frameID ) {
			break;
		}
	}
	std::ostringstream os;
	os << "Could not find frame " << frameID << " in [" << d_startFrame << ";" << d_endFrame << "]";
	throw std::out_of_range(os.str());
}

TrackingDataDirectory::const_iterator TrackingDataDirectory::FrameNear(const Time & t) const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}



}
}
}
