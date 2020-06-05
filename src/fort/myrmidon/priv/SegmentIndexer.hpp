#pragma once

#include "../Time.hpp"
#include "Types.hpp"
#include "FrameReference.hpp"

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
	typedef std::pair<FrameReference,T> Segment;

	void Insert(const FrameReference & ref, const T & value);

	void Insert(const Segment & s);

	std::vector<Segment> Segments() const;

	std::pair<FrameReference,T> Find(FrameID FID) const;

	std::pair<FrameReference,T> Find(const Time & t) const;

private:
	typedef std::shared_ptr<Segment> SegmentPtr;
	class FrameComparator {
	public:
		bool operator() (const uint64_t & a, const uint64_t & b) const;
	};


	class TimeComparator {
	public:
		bool operator() (const Time & a, const Time & b) const;
	};


	std::map<FrameID,SegmentPtr,FrameComparator> d_byID;
	std::map<Time,SegmentPtr,TimeComparator> d_byTime;

};


} //namespace priv

} //namespace myrmidon

} //namespace fort

#include "SegmentIndexer.impl.hpp"
