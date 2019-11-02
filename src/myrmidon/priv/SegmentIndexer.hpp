#pragma once

#include "../Time.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class SegmentIndexer {
public:

	typedef std::tuple<uint64_t,Time,std::string> Segment;

	void Insert(uint64_t FrameID, const Time & t, const std::string & value);


	std::vector<Segment> Segments() const;

	const std::string & Find(uint64_t FrameID) const;

	const std::string & Find(const Time & t) const;

private:

	std::map<uint64_t,std::string> d_byID;
	std::map<Time,std::string> d_byTime;

};


} //namespace priv

} //namespace myrmidon

} //namespace fort
