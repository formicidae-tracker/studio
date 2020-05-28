#pragma once

#include <functional>


#include "../Time.hpp"

class Defer {
public :
	static std::mutex PerfLock;

	Defer(const std::function<void()> & toDefer)
		: d_toDefer(toDefer) {
	}

	~Defer() {
		d_toDefer();
	}
private:
	std::function<void ()> d_toDefer;
};



#define PERF_FUNCTION() \
	auto __FM__perfStartTime = fort::myrmidon::Time::Now(); \
	Defer printTime([__FM__perfStartTime ]() { \
		                auto now = fort::myrmidon::Time::Now(); \
		                std::lock_guard<std::mutex> lock(Defer::PerfLock); \
		                std::cerr << __PRETTY_FUNCTION__ \
		                          << " took " \
		                          << now.Sub(__FM__perfStartTime) \
		                          << std::endl; \
	                })
