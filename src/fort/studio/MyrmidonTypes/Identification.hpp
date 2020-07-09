#pragma once

#include <QObject>

#include <fort/myrmidon/priv/Identification.hpp>

namespace fm = fort::myrmidon;
namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(fmp::Identification::Ptr)
Q_DECLARE_METATYPE(fmp::Identification::ConstPtr)
