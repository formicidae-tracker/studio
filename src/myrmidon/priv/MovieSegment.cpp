#include "MovieSegment.hpp"

#include <fstream>

#include "../utils/NotYetImplemented.hpp"



namespace fort {
namespace myrmidon {
namespace priv {


MovieSegment::Ptr MovieSegment::Open(const fs::path & moviePath,
                                     const fs::path & frameMatchingPath) {

	if ( fs::is_regular_file(frameMatchingPath) == false ) {
		throw std::invalid_argument("'" + frameMatchingPath.string() + "' is not a regular file");
	}

	ListOfOffset offsets;
	FrameID start(1),end(0);
	MovieFrameID movieStart,movieEnd;


	//TODO parse file
	std::ifstream in(frameMatchingPath.c_str());
	std::string line;
	FrameID lastOffset;
	while(std::getline(in,line)) {
		std::istringstream iss(line);
		FrameID trackingID;
		MovieFrameID movieID;
		iss >> movieID;
		if (!iss) {
			throw std::runtime_error("Could not read movie frame in line '" + line + "'");
		}
		iss >> trackingID;
		if (!iss) {
			throw std::runtime_error("Could not read tracking frame in line '" + line + "'");
		}
		FrameID currentOffset = trackingID - movieID;
		if ( offsets.empty() == true ) {
			start = trackingID;
			movieStart = movieID;
			lastOffset = currentOffset;
			offsets.push_back(std::make_pair(movieID,lastOffset));
		}
		if ( currentOffset != lastOffset ) {
			offsets.push_back(std::make_pair(movieID,currentOffset));
			lastOffset = currentOffset;
		}
		end = trackingID;
		movieEnd = movieID;
	}

	return std::make_shared<MovieSegment>(moviePath,
	                                      start,end,
	                                      movieStart,movieEnd,
	                                      offsets);
}


uint64_t MovieSegment::ToTrackingFrameID(uint64_t movieFrameID) const {
	if ( movieFrameID > d_movieEnd || movieFrameID < d_movieStart) {
		std::ostringstream oss;
		oss << movieFrameID << " is not in ["
		    << d_movieStart << ";" << d_movieEnd << "]";
		throw std::out_of_range(oss.str());
	}
	auto fi = d_byMovie.lower_bound(movieFrameID);

	if ( fi == d_byMovie.end() ) {
		throw std::logic_error("Internal error");
	}

	return movieFrameID + fi->second;
}

uint64_t MovieSegment::ToMovieFrameID(uint64_t trackingFrameID) const {
	if ( trackingFrameID > d_trackingEnd || trackingFrameID < d_trackingStart) {
		std::ostringstream oss;
		oss << trackingFrameID << " is not in ["
		    << d_trackingStart << ";" << d_trackingEnd << "]";
		throw std::out_of_range(oss.str());
	}
	auto fi = d_byTracking.lower_bound(trackingFrameID);

	if ( fi == d_byTracking.end() ) {
		throw std::logic_error("Internal error");
	}

	return trackingFrameID - fi->second;
}


MovieSegment::MovieSegment(const fs::path & path,
                           FrameID start,
                           FrameID end,
                           MovieFrameID startMovieID,
                           MovieFrameID endMovieID,
                           const ListOfOffset & offsets)
	: d_moviePath(path)
	, d_trackingStart(start)
	, d_trackingEnd(end)
	, d_movieStart(startMovieID)
	, d_movieEnd(endMovieID) {
	for (const auto & o : offsets) {
		d_byMovie.insert(o);
		d_byTracking.insert(std::make_pair(o.first+o.second,o.second));
	}
	if ( d_byMovie.count(startMovieID) == 0 ) {
		std::ostringstream oss;
		oss << startMovieID << " movie frame ID is missing from offsets lists";
		throw std::invalid_argument(oss.str());
	}
}

FrameID MovieSegment::StartFrame() const {
	return d_trackingStart;
}

FrameID MovieSegment::EndFrame() const {
	return d_trackingEnd;
}

MovieFrameID MovieSegment::StartMovieFrame() const {
	return d_movieStart;
}

MovieFrameID MovieSegment::EndMovieFrame() const {
	return d_movieEnd;
}

MovieSegment::ListOfOffset MovieSegment::Offsets() const {
	ListOfOffset offsets;
	for( const auto & o : d_byMovie) {
		offsets.push_back(o);
	}
	return offsets;
}

const fs::path & MovieSegment::MovieFilepath() const {
	return d_moviePath;
}

} // namespace fort
} // namespace myrmidon
} // namespace priv
