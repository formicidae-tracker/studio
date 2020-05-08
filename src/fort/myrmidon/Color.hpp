#pragma once

#include <tuple>

namespace fort {
namespace myrmidon {

// Defines a RGB color
//
// Defines a RGB color, specified in this order, i.e std::get<0>(c)
// will return the red component.
typedef std::tuple<uint8_t,uint8_t,uint8_t> Color;


// A collection of colors for visualization
//
// A Palette defines a collection of <Color>.
//
// The <Default> palette contains 7 color-blind friendly color.
class Palette {
public:
	// A color-blind friendly palette
	//
	// @return a Palette with 7 color-blind friendly <Color>
	static const Palette & Default();

	// Gets the color for given index.
	// @index the index in [0;<Size>[
	//
	// @return the <Color> at <index>
	virtual const Color & At(size_t index) const = 0;

	// The size of the Palette
	//
	// @return the number of <Color> in the Palette.
	virtual size_t Size() const = 0;
};



} // namespace myrmidon
} // namespace fort

// C++ Formatting operator for Color
// @out the <std::ostream> to format the color to
// @color the <fort::myrmidon::Color> to format
//
// @return a reference to <out>
std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Color & color);
