#pragma once

#if defined(NDEBUG)
#define FORT_STUDIO_VIDEO_PLAYER_NDEBUG 1
#endif

#include <fort/myrmidon/priv/Types.hpp>
#include <fort/myrmidon/Time.hpp>

#include <QObject>
#include <QImage>

#include <iostream>
#include <memory>

namespace fmp = fort::myrmidon::priv;
namespace fm = fort::myrmidon;

#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
#include <mutex>
#define VIDEO_PLAYER_DEBUG(statements) do {	  \
		std::lock_guard<std::mutex> debugLock(TrackingVideoFrame::debugMutex); \
		statements; \
	} while(0)
#else
#define VIDEO_PLAYER_DEBUG(statements)
#endif

struct TrackingVideoFrame {
	TrackingVideoFrame();
	std::shared_ptr<QImage>         Image;
	fm::Duration                    StartPos,EndPos;
	fmp::MovieFrameID               FrameID;
	fmp::IdentifiedFrame::ConstPtr  TrackingFrame;
	fmp::InteractionFrame::ConstPtr InteractionFrame;
#ifndef FORT_STUDIO_VIDEO_PLAYER_NDEBUG
	static std::mutex debugMutex;
#endif
};



Q_DECLARE_METATYPE(TrackingVideoFrame);

std::ostream & operator<<(std::ostream & out, const TrackingVideoFrame & f);
