#pragma once

#include <map>

#include <myrmidon/utils/FileSystem.hpp>

#include "Types.hpp"
#include "ForwardDeclaration.hpp"
#include "LocatableTypes.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class MovieSegment : public Identifiable, public FileSystemLocatable {
public:
	typedef uint32_t MovieID;
	typedef std::shared_ptr<MovieSegment> Ptr;
	typedef std::vector<Ptr>              List;

	typedef std::vector<std::pair<MovieFrameID,FrameID> > ListOfOffset;

	static Ptr Open(MovieID id,
	                const fs::path & moviePath,
	                const fs::path & frameMatchingPath,
	                const fs::path & parentURI);

	MovieSegment(MovieID id,
	             const fs::path & absoluteFilePath,
	             const fs::path & parentURI,
	             FrameID start,
	             FrameID end,
	             MovieFrameID startMovieID,
	             MovieFrameID endMovieID,
	             const ListOfOffset & offset);

	MovieID ID() const;

	FrameID StartFrame() const;
	FrameID EndFrame() const;

	MovieFrameID StartMovieFrame() const;
	MovieFrameID EndMovieFrame() const;

	ListOfOffset Offsets() const;

	const fs::path & AbsoluteFilePath() const override;
	const fs::path & URI() const override;

	uint64_t ToTrackingFrameID(uint64_t movieFrameID) const;

	uint64_t ToMovieFrameID(uint64_t trackingFrameID) const;

private:
	MovieID  d_ID;
	fs::path d_absoluteMovieFilePath;
	fs::path d_URI;
	FrameID d_trackingStart,d_trackingEnd;
	MovieFrameID d_movieStart,d_movieEnd;
	std::map<MovieFrameID,FrameID,std::greater<MovieFrameID> > d_byMovie;
	std::map<FrameID,FrameID,std::greater<MovieFrameID> >      d_byTracking;


};



} //namespace fort
} //namespace myrmidon
} //namespace priv
