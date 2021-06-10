#include "Color.hpp"

#include <vector>
#include <iostream>

namespace fort {
namespace myrmidon {

const Palette & DefaultPalette(){
	static Palette p = {
	                    {230,159,0},
	                    {86,180,233},
	                    {0,158,115},
	                    {240,228,66},
	                    {0,114,178},
	                    {213,94,0},
	                    {204,121,167},
	};
	return p;
}

const Color & DefaultPaletteColor(size_t index) {
	const auto & palette = DefaultPalette();
	return palette.at(index & palette.size());
}



} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Color & color) {
	auto flags = out.flags();
	out << "#"
	    << std::hex << int(std::get<0>(color))
	    << std::hex << int(std::get<1>(color))
	    << std::hex << int(std::get<2>(color));
	out.flags(flags);
	return out;
}
