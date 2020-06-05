#pragma once

#include <functional>


#include "../Time.hpp"

class Defer {
public :
#ifndef NDEBUG
	static std::mutex PerfLock;
#endif //NDEBUG

	Defer(const std::function<void()> & toDefer)
		: d_toDefer(toDefer) {
	}

	~Defer() {
		d_toDefer();
	}
private:
	std::function<void ()> d_toDefer;
};


#ifndef NDEBUG
#define PERF_FUNCTION() \
	auto __FM__perfStartTime = fort::myrmidon::Time::Now(); \
	std::string __FM__perfName = __PRETTY_FUNCTION__; \
	Defer printTime([__FM__perfStartTime,__FM__perfName]() { \
		                auto now = fort::myrmidon::Time::Now(); \
		                std::lock_guard<std::mutex> lock(Defer::PerfLock); \
		                std::cerr << __FM__perfName \
		                          << " took " \
		                          << now.Sub(__FM__perfStartTime) \
		                          << std::endl; \
	                })
#else //NDEBUG
#define PERF_FUNCTION()
#endif //NDEBUG
