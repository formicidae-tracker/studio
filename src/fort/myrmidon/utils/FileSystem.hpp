#pragma once

#include <fort/myrmidon/myrmidon-config.h>


#ifdef MYRMIDON_USE_BOOST_FILESYSTEM

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#else

#include <filesystem>

namespace fs = std::filesystem;

#endif
