#pragma once


#define FORT_MYRMIDON_CONST_HELPER(Class,Method,...)	  \
	reinterpret_cast<const Class *>(this)->Method(__VA_ARGS__)
