#pragma once


#include <system_error>

#include <fort/myrmidon/myrmidon-config.h>

#ifdef MYRMIDON_USE_SYSTEM_CATEGORY
#define MYRMIDON_SYSTEM_CATEGORY() std::system_category()
#else
#define MYRMIDON_SYSTEM_CATEGORY() std::generic_category()
#endif


#define MYRMIDON_SYSTEM_ERROR(fnct,err) std::system_error(err,MYRMIDON_SYSTEM_CATEGORY(),std::string("On call of ") + #fnct + "()")


#define p_call(fnct,...) do {	  \
		int myrmidon_pcall_res ## fnct = fnct(__VA_ARGS__); \
		if ( myrmidon_pcall_res ## fnct < 0 ) { \
			throw MYRMIDON_SYSTEM_ERROR(fnct,errno); \
		} \
	}while(0)



#define p_call_noerrno(fnct,...) do {	  \
		int myrmidon_pcall_res ## fnct = fnct(__VA_ARGS__); \
		if ( myrmidon_pcall_res ## fnct != 0 ) { \
			throw MYRMIDON_SYSTEM_ERROR(fnct,-myrmidon_pcall_res); \
		} \
	}while(0)
