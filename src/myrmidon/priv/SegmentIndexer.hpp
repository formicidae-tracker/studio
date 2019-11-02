#pragma once

#include "../Time.hpp"

namespace fort {

namespace myrmidon {

namespace priv {


// A SegementIndexer indexes segment
//
// A <SegementIndexer> indexes <Segement>: a <std::string> associated
// with a frame ID and a starting Time. Each segment is considered to
// finish when the next starts.
//
// <Insert> can be used to add a new segement to the index. \
//
// <Find> can be used to retrieve a segment from any frame number or
// <Time>
class SegmentIndexer {
public:

	typedef std::tuple<uint64_t,Time,std::string> Segment;

	void Insert(uint64_t frameID, const Time & t, const std::string & value);


	std::vector<Segment> Segments() const;

	const std::string & Find(uint64_t frameID) const;

	const std::string & Find(const Time & t) const;

private:
	class FrameComparator {
	public:
		bool operator() (const uint64_t & a, const uint64_t & b) const;
	};


	class TimeComparator {
	public:
		bool operator() (const Time & a, const Time & b) const;
	};


	std::map<uint64_t,std::shared_ptr<std::string>,FrameComparator> d_byID;
	std::map<Time,std::shared_ptr<std::string>,TimeComparator> d_byTime;

};


} //namespace priv

} //namespace myrmidon

} //namespace fort
