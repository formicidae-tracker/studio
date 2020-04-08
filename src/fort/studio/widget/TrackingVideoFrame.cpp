#include "TrackingVideoFrame.hpp"


std::ostream & operator<<(std::ostream & out, const TrackingVideoFrame & f) {
	return out << "{range:" <<f.StartPos << "-" <<  f.EndPos << ",data:" << f.Image.get() << "}";
}

#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
std::mutex TrackingVideoFrame::debugMutex;
#endif
