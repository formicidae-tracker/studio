#include "TrackingDataDirectory.hpp"
#include "FramePointer.hpp"

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
	if ( d_startFrame >= d_endFrame ) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startFrame:" << d_startFrame << " >= endDate: " << d_endFrame;
		throw std::invalid_argument(os.str());
	}

	if ( d_startDate >= d_endDate ) {
		std::ostringstream os;
		os << "TrackingDataDirectory: startDate:" << d_startDate << " >= endDate: " << d_endDate;
		throw std::invalid_argument(os.str());
	}
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

	uint64_t start,end;
	google::protobuf::Timestamp startDate,endDate;

	std::sort(hermesFiles.begin(),hermesFiles.end());

	fort::hermes::FrameReadout ro;
	try {
		fort::hermes::FileContext beginning(hermesFiles.front());
		beginning.Read(&ro);
		start = ro.frameid();
		startDate = ro.time();
	} catch ( const std::exception & e) {
		throw std::runtime_error("Could not extract first frame from " +  hermesFiles.front().string() + ": " + e.what());
	}

	try {
		fort::hermes::FileContext ending(hermesFiles.back());
		for (;;) {
			ending.Read(&ro);
			end = ro.frameid();
			endDate = ro.time();
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
	                             endDate);
}


FramePointer::Ptr TrackingDataDirectory::FramePointer(uint64_t frame) const {
	if ( frame < d_startFrame || frame > d_endFrame ) {
		std::ostringstream os;
		os << frame << " is outside of range ["
		   << d_startFrame << ";"
		   << d_endFrame << "] for tracking data directory "
		   << d_path;
		throw std::out_of_range(os.str());
	}
	auto res = std::make_shared<fort::myrmidon::priv::FramePointer>();
	res->Path = d_path;
	res->PathStartDate = d_startDate;
	res->Frame = frame;
	return res;
}

FramePointer::Ptr TrackingDataDirectory::FramePointer(const std::filesystem::path & path) const {
	if (path.parent_path() != d_path ) {
		std::ostringstream os;
		os << "Path:" << path << " does not match tracking data directory path " << d_path;
		throw std::invalid_argument(os.str());
	}
	std::istringstream iss(path.filename().generic_string());
	uint64_t frame;
	iss >> frame;
	if ( !iss.good() ) {
		std::ostringstream os;
		os << "could not parse frame number in " << path;
		throw std::invalid_argument(os.str());
	}

	return FramePointer(frame);
}
