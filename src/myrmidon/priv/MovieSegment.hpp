#pragma once

#include "ForwardDeclaration.hpp"

#include <map>

#include "../utils/FileSystem.hpp"

#include  "Types.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

class MovieSegment {
public:
	typedef std::shared_ptr<MovieSegment> Ptr;
	typedef std::vector<Ptr>              List;

	typedef std::vector<std::pair<MovieFrameID,FrameID> > ListOfOffset;

	static Ptr Open(const fs::path & moviePath,
	                const fs::path & frameMatchingPath);

	MovieSegment(const fs::path & path,
	             FrameID start,
	             FrameID end,
	             MovieFrameID startMovieID,
	             MovieFrameID endMovieID,
	             const ListOfOffset & offset);

	FrameID StartFrame() const;
	FrameID EndFrame() const;

	MovieFrameID StartMovieFrame() const;
	MovieFrameID EndMovieFrame() const;

	ListOfOffset Offsets() const;

	const fs::path & MovieFilepath() const;

	uint64_t ToTrackingFrameID(uint64_t movieFrameID) const;

	uint64_t ToMovieFrameID(uint64_t trackingFrameID) const;

private:

	fs::path d_moviePath;
	FrameID d_trackingStart,d_trackingEnd;
	MovieFrameID d_movieStart,d_movieEnd;
	std::map<MovieFrameID,FrameID,std::greater<MovieFrameID> > d_byMovie;
	std::map<FrameID,FrameID,std::greater<MovieFrameID> >      d_byTracking;


};



} //namespace fort
} //namespace myrmidon
} //namespace priv
