#pragma once

#include "../Time.hpp"
#include "Types.hpp"

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
template <typename T>
class SegmentIndexer {
public:
	typedef std::shared_ptr<SegmentIndexer> Ptr;
	typedef std::shared_ptr<const SegmentIndexer> ConstPtr;
	typedef std::tuple<uint64_t,Time,T> Segment;

	void Insert(FrameID FID, const Time & t, const T & value);


	std::vector<Segment> Segments() const;

	const T & Find(FrameID FID) const;

	const T & Find(const Time & t) const;

private:
	class FrameComparator {
	public:
		bool operator() (const uint64_t & a, const uint64_t & b) const;
	};


	class TimeComparator {
	public:
		bool operator() (const Time & a, const Time & b) const;
	};


	std::map<uint64_t,std::shared_ptr<T>,FrameComparator> d_byID;
	std::map<Time,std::shared_ptr<T>,TimeComparator> d_byTime;

};


} //namespace priv

} //namespace myrmidon

} //namespace fort

#include "SegmentIndexer.impl.hpp"
