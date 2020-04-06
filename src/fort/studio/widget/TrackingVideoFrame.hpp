#pragma once

#include <fort/myrmidon/priv/Types.hpp>

#include <QObject>

namespace fmp = fort::myrmidon::priv;

class QOpenGLBuffer;


struct TrackingVideoFrame {
	QOpenGLBuffer                * Buffer;
	int                            Width,Height;
	qint64                         PositionMS;
	fmp::MovieFrameID              FrameID;
	fmp::IdentifiedFrame::ConstPtr TrackingFrame;
};

Q_DECLARE_METATYPE(TrackingVideoFrame);
