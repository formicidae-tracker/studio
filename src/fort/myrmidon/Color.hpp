#pragma once

#include <tuple>

namespace fort {
namespace myrmidon {

typedef std::tuple<uint8_t,uint8_t,uint8_t> Color;

class Palette {
public:
	static const Palette & Default();

	virtual const Color & At(size_t index) const = 0;

	virtual size_t Size() const = 0;
};



} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Color & color);
