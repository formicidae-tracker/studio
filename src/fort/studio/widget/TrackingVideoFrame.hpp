#pragma once

#include <fort/myrmidon/priv/Types.hpp>
#include <fort/myrmidon/Time.hpp>

#include <QObject>
#include <QImage>
#include <mutex>

#include <iostream>

namespace fmp = fort::myrmidon::priv;
namespace fm = fort::myrmidon;



struct TrackingVideoFrame {
	QImage                       * Image;
	fm::Duration                   StartPos,EndPos;
	fmp::MovieFrameID              FrameID;
	fmp::IdentifiedFrame::ConstPtr TrackingFrame;
	static std::mutex debugMutex;
};



Q_DECLARE_METATYPE(TrackingVideoFrame);

std::ostream & operator<<(std::ostream & out, const TrackingVideoFrame & f);
