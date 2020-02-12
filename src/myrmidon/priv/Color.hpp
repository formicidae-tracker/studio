#pragma once

#include <tuple>

namespace fort {
namespace myrmidon {
namespace priv {

typedef std::tuple<uint8_t,uint8_t,uint8_t> Color;

class ColorMap {
public:
	static const ColorMap & Default();

	virtual const Color & ColorAt(size_t index) const = 0;

	virtual size_t Size() const = 0;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
