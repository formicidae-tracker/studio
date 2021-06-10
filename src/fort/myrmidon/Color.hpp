#pragma once

#include <tuple>
#include <vector>

namespace fort {
namespace myrmidon {

/**
 * Defines a RGB color.
 *
 * * Python: `py_fort_myrmidon.Color` class constructible from a 3 `int` `tuple`
 * * R:
 * ```R
 * fmColor <- function(color = c(255,255,255)) # returns a Rcpp_fmColor.
 * ```
 *
 * Color are RGB triplet (stored in RGB order). `fort-myrmidon` also
 * define a Palette::Default() of color-blind friendly colors.
 */
typedef std::tuple<uint8_t,uint8_t,uint8_t> Color;


/**
 * A Palette defines a collection of Color.
 *
 * * Python: a `list` of `py_fort_myrmidon.Color`
 * * R: a `slist` of `Rcpp_fmColor`
 */
typedef std::vector<Color> Palette;


/**
 * A Palette of 7 color-blind friendly colors for visualiztion.
 *
 * We use the color set from [Wong 2011: Nature methods 8:441].
 *
 */
const Palette & DefaultPalette();


} // namespace myrmidon
} // namespace fort

/**
 * C++ Formatting operator for Color
 * @param out the std::ostream to format the color to
 * @param color the fort::myrmidon::Color to format
 *
 * @return a reference to out
 */
std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::Color & color);
