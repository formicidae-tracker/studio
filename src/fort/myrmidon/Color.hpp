#pragma once

#include <tuple>
#include <vector>

namespace fort {
namespace myrmidon {

/**
 * Defines a RGB color.
 *
 * * Python: translate to a `Tuple[int,int,int]`
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
 * * Python: translate to `List[Tuple[int,int,int]]`
 * * R: a `slist` of `Rcpp_fmColor`
 */
typedef std::vector<Color> Palette;


/**
 * A Palette of 7 color-blind friendly colors for visualiztion.
 *
 * * Python:
 * ```python
 * py_fort_myrmidon.DefaultPalette() -> List[Tuple[int,int,int]]
 * ```
 * * R:
 * ```R
 * fmDefaultPalette <- function() # returns a slist of Rcpp_fmColor
 * ```
 *
 * We use the color set from [Wong 2011: Nature methods 8:441].
 *
 */
const Palette & DefaultPalette();

/**
 * Safely access a color from the DefaultPalette()
 *
 * * Python:
 * ```python
 * py_fort_myrmidon.DefaultPaletteColor(index: int) -> Tuple[int,int,int]
 * ```
 * * R:
 * ```R
 * fmDefaultPaletteColor <- function(index = 0) # returns a Rcpp_fmColor
 * ```
 *
 * It is a safe version of
 * `fort::myrmidon::DefaultPalette().at(index)` as color will be
 * wrapped around.
 *
 * @param index a wrapped around index of the wanted Color in the Palette
 *
 * @return the index-th Color of the Palette, wrapped around if `index
 *         >= DefaultPalette().size()`
 */
const Color & DefaultPaletteColor(size_t index);

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
