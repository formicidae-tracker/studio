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
		// We use the color set from [Wong 2011: Nature methods 8:441],
		// which is a 7 color bright color map more or less color-blind
		// friendly.
		d_colors = {
		            {230,159,0},
		            {86,180,233},
		            {0,158,115},
		            {240,228,66},
		            {0,114,178},
		            {213,94,0},
		            {204,121,167},
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
