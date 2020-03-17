#include "Color.hpp"

#include <vector>
#include <iostream>

namespace fort {
namespace myrmidon {
namespace priv {

typedef std::tuple<uint8_t,uint8_t,uint8_t> Color;

class DefaultPalette : public Palette {
	std::vector<Color> d_colors;

public:
	DefaultPalette() {
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

	const Color & At(size_t index) const override {
		return d_colors[index % d_colors.size()];
	}

	size_t Size() const override {
		return d_colors.size();
	}

};

const Palette & Palette::Default() {
	static DefaultPalette defaultPalette;
	return defaultPalette;
}

} // namespace priv
} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::Color & color) {
	auto flags = out.flags();
	out << "#"
	    << std::hex << int(std::get<0>(color))
	    << std::hex << int(std::get<1>(color))
	    << std::hex << int(std::get<2>(color));
	out.flags(flags);
	return out;
}
