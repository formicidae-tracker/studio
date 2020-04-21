#pragma once

#include <string>
#include <QObject>
#include <QColor>
#include <QIcon>

#include <fort/tags/fort-tags.h>
#include <fort/myrmidon/priv/Ant.hpp>
#include <fort/myrmidon/priv/AntShapeType.hpp>
#include <fort/myrmidon/priv/Identification.hpp>
#include <fort/myrmidon/priv/Space.hpp>
#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>
#include <fort/myrmidon/priv/TagCloseUp.hpp>
#include <fort/myrmidon/Time.hpp>
#include <fort/myrmidon/priv/Measurement.hpp>
#include <fort/myrmidon/priv/Color.hpp>
#include <fort/myrmidon/priv/AntMetadata.hpp>
#include <fort/myrmidon/priv/MovieSegment.hpp>
#include <fort/myrmidon/priv/Experiment.hpp>

namespace fm = fort::myrmidon;
namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(fort::tags::Family)
Q_DECLARE_METATYPE(fmp::Ant::Ptr)
Q_DECLARE_METATYPE(fmp::Ant::ConstPtr)
Q_DECLARE_METATYPE(fmp::Ant::DisplayState)
Q_DECLARE_METATYPE(fmp::AntShapeType::Ptr)
Q_DECLARE_METATYPE(fmp::Color)
Q_DECLARE_METATYPE(fmp::Identification::Ptr)
Q_DECLARE_METATYPE(fmp::Identification::ConstPtr)
Q_DECLARE_METATYPE(fmp::TrackingDataDirectory::ConstPtr)
Q_DECLARE_METATYPE(fmp::Space::Ptr)
Q_DECLARE_METATYPE(fm::Time::ConstPtr)
Q_DECLARE_METATYPE(fm::Time)
Q_DECLARE_METATYPE(fm::Duration)
Q_DECLARE_METATYPE(fmp::TagCloseUp::ConstPtr)
Q_DECLARE_METATYPE(fmp::MeasurementType::Ptr)
Q_DECLARE_METATYPE(fmp::AntMetadata::Column::Ptr)
Q_DECLARE_METATYPE(fmp::MovieSegment::ConstPtr)
Q_DECLARE_METATYPE(fmp::Experiment::ConstPtr)
Q_DECLARE_METATYPE(fmp::Zone::Ptr)
Q_DECLARE_METATYPE(fmp::Zone::Definition::Ptr)


class Conversion {
public:
	static QColor colorFromFM(const fmp::Color & color, int opacity = 255);
	static QIcon  iconFromFM(const fmp::Color & color);
	static QPointF fromEigen(const Eigen::Vector2d & p);
	static Eigen::Vector2d toEigen(const QPointF  & p);
};
