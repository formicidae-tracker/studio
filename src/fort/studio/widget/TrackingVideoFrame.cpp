#include "TrackingVideoFrame.hpp"

TrackingVideoFrame::TrackingVideoFrame()
	: FrameID(std::numeric_limits<fmp::MovieFrameID>::max()) {
}

std::ostream & operator<<(std::ostream & out, const TrackingVideoFrame & f) {
	return out << "{ID:"<< f.FrameID << ",range:" <<f.StartPos << "-" <<  f.EndPos << ",data:" << f.Image.get() << "}";
}

#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
std::mutex TrackingVideoFrame::debugMutex;
#endif
