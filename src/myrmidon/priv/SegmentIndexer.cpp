#include "SegmentIndexer.hpp"

#include "../utils/NotYetImplemented.hpp"

namespace fort {

namespace myrmidon {

namespace priv {


void SegmentIndexer::Insert(uint64_t FrameID, const Time & t, const std::string & value) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

std::vector<SegmentIndexer::Segment> SegmentIndexer::Segments() const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

const std::string & SegmentIndexer::Find(uint64_t FrameID) const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

const std::string & SegmentIndexer::Find(const Time & t) const {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}


} //namespace priv

} //namespace myrmidon

} //namespace fort
