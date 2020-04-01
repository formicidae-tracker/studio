#pragma once

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
Q_DECLARE_METATYPE(fmp::TrackingDataDirectory::ConstPtr);
Q_DECLARE_METATYPE(fmp::Space::Ptr);
Q_DECLARE_METATYPE(fm::Time::ConstPtr)
Q_DECLARE_METATYPE(fmp::TagCloseUp::ConstPtr)
Q_DECLARE_METATYPE(fmp::MeasurementType::Ptr)
Q_DECLARE_METATYPE(fmp::AntMetadata::Column::Ptr)
Q_DECLARE_METATYPE(fmp::MovieSegment::ConstPtr)
