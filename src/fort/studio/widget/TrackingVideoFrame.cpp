#include "TrackingVideoFrame.hpp"


std::ostream & operator<<(std::ostream & out, const TrackingVideoFrame & f) {
	return out << "{range:" <<f.StartPos << "-" <<  f.EndPos << ",data:" << f.Image << "}";
}
