#pragma once


#include <system_error>

#include <myrmidion-config.h>

#ifdef MYRMIDION_USE_SYSTEM_CATEGORY
#define MYRMIDION_SYSTEM_CATEGORY() std::system_category()
#else
#define MYRMIDION_SYSTEM_CATEGORY() std::generic_category()
#endif


#define MYRMIDION_SYSTEM_ERROR(fnct,err) std::system_error(err,MYRMIDION_SYSTEM_CATEGORY(),std::string("On call of ") + #fnct + "()")


#define p_call(fnct,...) do {	  \
		int myrmidion_pcall_res ## fnct = fnct(__VA_ARGS__); \
		if ( myrmidion_pcall_res ## fnct < 0 ) { \
			throw MYRMIDION_SYSTEM_ERROR(fnct,errno); \
		} \
	}while(0)



#define p_call_noerrno(fnct,...) do {	  \
		int myrmidion_pcall_res ## fnct = fnct(__VA_ARGS__); \
		if ( myrmidion_pcall_res ## fnct != 0 ) { \
			throw MYRMIDION_SYSTEM_ERROR(fnct,-myrmidion_pcall_res); \
		} \
	}while(0)
