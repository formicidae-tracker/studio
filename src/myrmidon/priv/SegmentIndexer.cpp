#include "SegmentIndexer.hpp"

#include "../utils/NotYetImplemented.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

bool SegmentIndexer::FrameComparator::operator() (const uint64_t & a, const uint64_t & b) const {
	// we store the map in inverse order so lower_bound return smaller or equal values
	return a > b;
}

bool  SegmentIndexer::TimeComparator::operator() (const Time & a, const Time & b) const {
	// we store the map in inverse order so lower_bound return smaller or equal values
	return a.After(b);
}



void SegmentIndexer::Insert(uint64_t frameID, const Time & t, const std::string & value) {
	//makes some test to respect invariant
	auto ffi = d_byID.lower_bound(frameID);
	auto tfi = d_byTime.lower_bound(t);
	if ( (ffi != d_byID.end() && tfi == d_byTime.end() ) ||
	     (ffi == d_byID.end() && tfi != d_byTime.end() ) ||
	     (ffi != d_byID.end() && tfi != d_byTime.end() && ffi->second.get() != tfi->second.get() ) ) {
		std::ostringstream os;
		os << "Wanted segment timing {Frame: " << frameID;
		if ( ffi != d_byID.end() ) {
			os << "(previous: " << ffi->first << ") ";
		} else {
			os << "(no previous frame) ";
		}
		os << "Time: " << t;
		if ( tfi != d_byTime.end() ) {
			os << "(previous: " << tfi->first << ")";
		} else {
			os << "(no previous time)";
		}
		os << "} is inconsistent with internal data";
		throw std::invalid_argument(os.str());
	}

	auto valueptr = std::make_shared<std::string>(value);

	d_byID.insert(std::make_pair(frameID,valueptr));
	d_byTime.insert(std::make_pair(t,valueptr));
}

std::vector<SegmentIndexer::Segment> SegmentIndexer::Segments() const {
	std::vector<Segment> res(d_byTime.size());
	std::vector<std::shared_ptr<std::string> > resPtr(d_byTime.size());
	size_t i = res.size();
	for ( const auto & [t,value] : d_byTime ) {
		--i;
		std::get<1>(res[i]) = t;
		resPtr[i] = value;
	}
	i = res.size();
	for (const auto & [id,value] : d_byID ) {
		--i;
		if ( resPtr[i].get() != value.get()  ) {
			throw std::logic_error("Keys where not ordered appropriately");
		}
		std::get<0>(res[i]) = id;
	}

	for ( i = 0; i < resPtr.size(); ++i ) {
		std::get<2>(res[i]) = *resPtr[i];
	}


	return res;
}

const std::string & SegmentIndexer::Find(uint64_t frameID) const {
	auto fi = d_byID.lower_bound(frameID);
	if ( fi == d_byID.end() ) {
		std::ostringstream os;
		os << frameID << " is too small";
		throw std::out_of_range(os.str());
	}
	return *fi->second;
}

const std::string & SegmentIndexer::Find(const Time & t) const {
	auto fi = d_byTime.lower_bound(t);
	if ( fi == d_byTime.end() ) {
		std::ostringstream os;
		os << t << " is too small";
		throw std::out_of_range(os.str());
	}

	return *fi->second;
}


} //namespace priv

} //namespace myrmidon

} //namespace fort
