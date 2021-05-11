#pragma once

#include <fort/hermes/FrameReadout.pb.h>
#include <fort/time/Time.hpp>


namespace fort {
namespace myrmidon {
namespace priv {

inline static
Time TimeFromFrameReadout(const fort::hermes::FrameReadout & ro, Time::MonoclockID monoID) {
	if (ro.error() !=  fort::hermes::FrameReadout_Error_NO_ERROR || ro.timestamp() == 0) {
		return Time::FromTimestamp(ro.time());
	}
	return Time::FromTimestampAndMonotonic(ro.time(),ro.timestamp() * 1000, monoID);
}


} //namespace priv
} //namespace myrmidon
} //namespace fort
