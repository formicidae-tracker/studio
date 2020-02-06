#pragma once

#include "FileSystem.hpp"
#include <stdexcept>


#define FORT_MYRMIDON_CHECK_PATH_IS_ABSOLUTE(varName) do {	  \
		if ( varName.is_absolute() == false ) { \
			throw std::invalid_argument( #varName ":'" \
			                             + varName.string() \
			                             + "' is not absolute"); \
		} \
	} while(0)
