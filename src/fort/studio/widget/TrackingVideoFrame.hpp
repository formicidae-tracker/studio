#pragma once

#include <fort/myrmidon/priv/Types.hpp>

#include <QObject>
#include <QImage>

namespace fmp = fort::myrmidon::priv;



struct TrackingVideoFrame {
	QImage                       * Image;
	qint64                         StartMS,EndMS;
	fmp::MovieFrameID              FrameID;
	fmp::IdentifiedFrame::ConstPtr TrackingFrame;
};

Q_DECLARE_METATYPE(TrackingVideoFrame);
