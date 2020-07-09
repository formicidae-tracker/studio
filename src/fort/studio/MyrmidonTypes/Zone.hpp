#pragma once

#include <QObject>

#include <fort/myrmidon/priv/Zone.hpp>

namespace fm = fort::myrmidon;
namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(fmp::Zone::Ptr)
Q_DECLARE_METATYPE(fmp::Zone::Definition::Ptr)
