#include "Color.hpp"

#include <vector>

namespace fort {
namespace myrmidon {
namespace priv {

typedef std::tuple<uint8_t,uint8_t,uint8_t> Color;

class DefaultColorMap : public ColorMap {
	std::vector<Color> d_colors;

public:
	DefaultColorMap() {
		d_colors = {
		            {255,0,0},
		            {0,255,0},
		            {0,0,255},
		            {255,255,0},
		            {255,0,255},
		            {255,255,0},
		};

	}

	const Color & ColorAt(size_t index) const override {
		return d_colors[index % d_colors.size()];
	}

	size_t Size() const override {
		d_colors.size();
	}

};

const ColorMap & ColorMap::Default() {
	static DefaultColorMap defaultMap;
	return defaultMap;
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
