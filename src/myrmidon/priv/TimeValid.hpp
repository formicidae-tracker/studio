#pragma once


#include "../Time.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

// Represents something valid in Time
//
// Represents something valid in a <Time> range
// [<d_start>;<d_end>[. <IsValid> can be used to query if this object
// is actually valid for a given <Time>. <d_start> and <d_end> can be
// unset pointer to represent respectively -∞ and +∞.
//
// <SortAndCheckOverlap>, <UpperUnvalidBound> and <LowerUnvalidBound>
// are utility function that operate on collection of <TimeValid>
// objects.
class TimeValid {
public:
	// A needed virtual destructor
	virtual ~TimeValid() {}

	// Informs if object is valid for a given time
	// @time the <Time> to test against
	// @return true if <t> ∈ [ <d_start>, <d_end> [
	inline bool IsValid(const Time & time) const {
		return ( !d_start || !time.Before(*d_start) ) &&
			( !d_end || time.Before(*d_end));
	}

	// Sorts a collection and return first time-overlapping objects
	// @InputIt the iterator type
	// @begin the start of the range
	// @end the end of the range
	// @return iterator to the first pair
	//
	template <typename InputIt>
	static std::pair<InputIt,InputIt>
	SortAndCheckOverlap(InputIt begin,InputIt end) {
		std::sort(begin,end,
		          [](const auto & a,
		             const auto & b) -> bool {
			          if (!((a)->d_start) ) {
				          return true;
			          }

			          if (!((b)->d_start) ) {
				          return false;
			          }

			          return (a)->d_start->Before(*((b)->d_start));
		          });

		if ( std::distance(begin,end) < 2 ) {
			return std::make_pair(InputIt(),InputIt());
		}

		auto prev = begin;
		for ( auto i = begin + 1;
		      i != end;
		      ++i) {

			// if end == start, we are good as validity range is end opened ([start,end[)
			if ( !((*i)->d_start) || !((*prev)->d_end) || ((*prev)->d_end->After(*((*i)->d_start))) ) {
				return std::make_pair(prev,i);
			}

			prev = i;
		}
		return std::make_pair(InputIt(),InputIt());
	}


	// Finds the next time an object is valid
	// @InputIt the iterator type of the collection
	// @t the time to test for
	// @begin the start of the range to test
	// @end the end of the range to test
	// @return the next time after <t> where an object in
	//         [<start>,<end>[ is valid. An empty pointer if this is
	//         +∞.
	//
	// Finds the prior time after <t> where an object in the
	// collection [<start>,<end>[ is valid. This could be +∞, i.e. an
	// empty pointer.  Throws std::invalid_argument if t is valid for
	// any object in [start;end[.
	template <typename InputIt>
	static Time::ConstPtr UpperUnvalidBound(const Time & t, InputIt begin, InputIt end) {
		for(;begin != end; ++begin) {
			if ( (*begin)->IsValid(t) == true ) {
				std::ostringstream os;
				os << t << " is valid for " << **begin;
				throw std::invalid_argument(os.str());
			}
			if ( !(*begin)->d_start ) {
				continue;
			}
			if ( t.Before(*((*begin)->d_start)) ) {
				return (*begin)->d_start;
			}
		}
		return Time::ConstPtr();
	}

	// Finds the prior time an object is valid
	// @InputIt the iterator type of the collection
	// @t the time to test for
	// @begin the start of the range to test
	// @end the end of the range to test
	// @return the next time after <t> where an object in
	//         [<start>,<end>[ is valid. An empty pointer if this is
	//         +∞.
	//
	// Finds the prior time before <t> where an object in the
	// collection [<start>,<end>[ is valid. This could be +∞, i.e. an
	// empty pointer.  Throws std::invalid_argument if t is valid for
	// any object in [start;end[.
	template <typename InputIt>
	static Time::ConstPtr LowerUnvalidBound(const Time & t, InputIt begin, InputIt end) {
		for(InputIt rit = end-1; rit != begin-1; --rit) {
			if ( (*rit)->IsValid(t) == true ) {
				std::ostringstream os;
				os << t << " is valid for " << **rit;
				throw std::invalid_argument(os.str());
			}
			if ( !(*rit)->d_end ) {
				continue;
			}
			if ( !t.Before(*((*rit)->d_end)) ) {
				return (*rit)->d_end;
			}
		}
		return Time::ConstPtr();
	}


protected:

	Time::ConstPtr d_start;
	Time::ConstPtr d_end;
};

} // namespace priv

} // namespace myrmidon

} // namespace fort
