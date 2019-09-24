#include "TrackingDataDirectory.hpp"

#include "Experiment.pb.h"

#include <fort-hermes/Error.h>
#include <fort-hermes/FileContext.h>

using namespace fort::myrmidon::priv;
namespace fs = std::filesystem;

TrackingDataDirectory::TrackingDataDirectory()
	: StartFrame(0)
	, EndFrame(0) {
}


TrackingDataDirectory TrackingDataDirectory::FromSaved(const pb::TrackingDataDirectory & tdd) {
	TrackingDataDirectory res;
	res.Path = tdd.path();
	res.StartFrame = tdd.startframe();
	res.EndFrame = tdd.endframe();
	res.StartDate.CheckTypeAndMergeFrom(tdd.startdate());
	res.EndDate.CheckTypeAndMergeFrom(tdd.enddate());
	return res;
}



TrackingDataDirectory TrackingDataDirectory::Open(const std::filesystem::path & path, const std::filesystem::path & base) {
	if ( fs::is_directory(base) ) {
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
		tdd.StartFrame = ro.frameid();
		tdd.StartDate.Clear();
		tdd.StartDate.CheckTypeAndMergeFrom(ro.time());
	} catch ( const std::exception & e) {
		throw std::runtime_error("Could not extract first frame from " +  hermesFiles.front().string() + ": " + e.what());
	}

	try {
		fort::hermes::FileContext ending(hermesFiles.back());
		for (;;) {
			ending.Read(&ro);
			tdd.EndFrame = ro.frameid();
			tdd.EndDate.CheckTypeAndMergeFrom(ro.time());
		}

	} catch ( const fort::hermes::EndOfFile &) {
		//DO nothing, we just reached EOF
	} catch ( const std::exception & e) {
		throw std::runtime_error("Could not extract last frame from " +  hermesFiles.back().string() + ": " + e.what());
	}

	tdd.Path = fs::relative(path,base);

	return tdd;
}
