#pragma once

#include <functional>


class Defer {
public :
	Defer(const std::function<void()> & toDefer)
		: d_toDefer(toDefer) {
	}

	~Defer() {
		d_toDefer();
	}
private:
	std::function<void ()> d_toDefer;
};
