#pragma once

#include <fort/tags/fort-tags.h>
#include <myrmidon/priv/Ant.hpp>
#include <myrmidon/priv/AntShapeType.hpp>
#include <myrmidon/priv/Identification.hpp>
#include <myrmidon/priv/Space.hpp>
#include <myrmidon/priv/TrackingDataDirectory.hpp>
#include <myrmidon/priv/TagCloseUp.hpp>
#include <myrmidon/Time.hpp>
#include <myrmidon/priv/Measurement.hpp>
#include <myrmidon/priv/Color.hpp>

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
