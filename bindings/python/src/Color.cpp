#include <sstream>
#include <iomanip>

#include <pybind11/stl.h>

#include "BindMethods.hpp"

#include <fort/myrmidon/Color.hpp>

namespace py = pybind11;


void BindColor(py::module_ & m) {
	using namespace fort::myrmidon;

	m.def("DefaultPalette",
	      &DefaultPalette,
	      R"pydoc(
    Returns `fort-myrmidon` default palette.

    In `py_fort_myrmidon`, a Color is simply a `Tuple[int, int, int]`.
    `Tuple[int, int, int]`. The default palette are 7 distinguishable
    color, which are color-blind friendly.

    Returns:
         List[Tuple[int,int,int]]: 7 color-blind friendly colors.
  )pydoc");

	m.def("DefaultPaletteColor",
	      &DefaultPaletteColor,
	      py::arg("index"),
	      R"pydoc(
    Safely returns a `fort-myrmidon` default palette color's from any index

    Args:
        index(int) : a positive index

    Returns:
        Tuple[int,int,int]: a Color from DefaultPalette, safely
             wrapping around if index >= len(DefaultPalette())
.)pydoc");


}
