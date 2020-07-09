#pragma once

#include <QObject>

#include <fort/myrmidon/priv/Ant.hpp>

namespace fm = fort::myrmidon;
namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(fmp::Ant::Ptr)
Q_DECLARE_METATYPE(fmp::Ant::ConstPtr)
Q_DECLARE_METATYPE(fmp::Ant::DisplayState)
