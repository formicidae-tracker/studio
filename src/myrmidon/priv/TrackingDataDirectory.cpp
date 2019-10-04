#include "TrackingDataDirectory.hpp"

#include <fort-hermes/Error.h>
#include <fort-hermes/FileContext.h>

using namespace fort::myrmidon::priv;
namespace fs = std::filesystem;

TrackingDataDirectory::TrackingDataDirectory()
	: d_startFrame(0)
	, d_endFrame(0) {
}

TrackingDataDirectory::TrackingDataDirectory(const std::filesystem::path & path,
                                             uint64_t startFrame,
                                             uint64_t endFrame,
                                             const google::protobuf::Timestamp & startdate,
                                             const google::protobuf::Timestamp & enddate)
	: d_path(path)
	, d_startFrame(startFrame)
	, d_endFrame(endFrame)
	, d_startDate(startdate)
	, d_endDate(enddate) {
}


const std::filesystem::path &  TrackingDataDirectory::Path() const {
	return d_path;
}

uint64_t TrackingDataDirectory::StartFrame() const {
	return d_startFrame;
}

uint64_t TrackingDataDirectory::EndFrame() const {
	return d_endFrame;
}

const google::protobuf::Timestamp & TrackingDataDirectory::StartDate() const {
	return d_startDate;
}

const google::protobuf::Timestamp & TrackingDataDirectory::EndDate() const {
	return d_endDate;
}



TrackingDataDirectory TrackingDataDirectory::Open(const std::filesystem::path & path, const std::filesystem::path & base) {
	if ( fs::is_directory(base) == false ) {
		throw std::invalid_argument("base path " + base.string() +  " is not a directory");
	}
	if ( fs::is_directory(path) == false ) {
		throw std::invalid_argument( path.string() + " is not a directory");
	}

	TrackingDataDirectory tdd;

	std::vector<fs::path> hermesFiles;

	for( auto const & f : fs::directory_iterator(path) ) {
		if ( f.is_regular_file() == false ) {
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

	std::sort(hermesFiles.begin(),hermesFiles.end());

	fort::hermes::FrameReadout ro;
	try {
		fort::hermes::FileContext beginning(hermesFiles.front());
		beginning.Read(&ro);
		tdd.d_startFrame = ro.frameid();
		tdd.d_startDate.Clear();
		tdd.d_startDate.CheckTypeAndMergeFrom(ro.time());
	} catch ( const std::exception & e) {
		throw std::runtime_error("Could not extract first frame from " +  hermesFiles.front().string() + ": " + e.what());
	}

	try {
		fort::hermes::FileContext ending(hermesFiles.back());
		for (;;) {
			ending.Read(&ro);
			tdd.d_endFrame = ro.frameid();
			tdd.d_endDate.CheckTypeAndMergeFrom(ro.time());
		}

	} catch ( const fort::hermes::EndOfFile &) {
		//DO nothing, we just reached EOF
	} catch ( const std::exception & e) {
		throw std::runtime_error("Could not extract last frame from " +  hermesFiles.back().string() + ": " + e.what());
	}

	tdd.d_path = fs::relative(path,base);

	return tdd;
}
