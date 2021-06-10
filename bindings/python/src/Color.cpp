#include <sstream>
#include <iomanip>

#include <pybind11/stl.h>

#include "BindMethods.hpp"

#include <fort/myrmidon/Color.hpp>

namespace py = pybind11;

#define cast(var,index) int var; \
	do { \
		var = color[index].cast<int>(); \
	} while(0)

#define check(var) do { \
		if ( var < 0 || var > 255 ) { \
			throw std::invalid_argument("invalid value " \
			                            + std::to_string(var) \
			                            + " for channel " #var); \
		} \
	} while(0)


#define cast_and_check(var,index) cast(var,index); check(var)

template<std::size_t I = 0, typename U, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), U>::type
get_index(int, const std::tuple<Tp...> &) {
	return 0;
}

template<std::size_t I = 0, typename U, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), U>::type
get_index(int index, const std::tuple<Tp...>& t) {
	if (index == 0) {
		return std::get<I>(t);
	}
	return get_index<I + 1, U, Tp...>(index-1, t);
}


void BindColor(py::module_ & m) {
	using namespace fort::myrmidon;

	class ColorP : public Color {
	public:
		ColorP(const Color & color) : Color(color) {}
		py::tuple AsTuple() const {
			return py::make_tuple(std::get<0>(*this),std::get<1>(*this),std::get<2>(*this));
		}
		std::string Debug() const {
			return "py_fort_myrmidon.Color(R = "
				+ std::to_string(std::get<0>(*this))
				+ ", G = "
				+ std::to_string(std::get<1>(*this))
				+ ", B = "
				+ std::to_string(std::get<2>(*this))
				+ ")";
		}

		std::string Format() const {
			std::ostringstream oss;
			oss << "#" << std::hex << std::uppercase << std::setfill('0')
			    << std::setw(2) << int(std::get<0>(*this))
			    << std::setw(2) << int(std::get<1>(*this))
			    << std::setw(2) << int(std::get<2>(*this));
			return oss.str();
		}

	};

	py::class_<ColorP>(m,"Color")
		.def(py::init([](const py::tuple & color) -> ColorP {
			              if ( color.size() != 3 ) {
				              throw std::invalid_argument("invalid number of channel "
				                                          + std::to_string(color.size())
				                                          + " expected 3");
			              }
			              cast_and_check(R,0);
			              cast_and_check(G,1);
			              cast_and_check(B,2);
			              return ColorP({R,G,B});
		              }))
		.def(py::init([](int R, int G, int B) -> Color {
			              check(R);
			              check(G);
			              check(B);
			              return ColorP({R,G,B});
		              }),
			py::arg("R"),py::arg("G"),py::arg("B"))
		.def("AsTuple",&ColorP::AsTuple)
		.def("__repr__",&ColorP::Debug)
		.def("__str__",&ColorP::Format)
		;

	py::implicitly_convertible<py::tuple,ColorP>();

	m.def("DefaultPalette",
	      &DefaultPalette,
	      R"pydoc(returns `fort-myrmidon` default palette.)pydoc");

	m.def("DefaultPaletteColor",
	      &DefaultPaletteColor,
	      py::arg("index"),
	      R"pydoc(safely returns `fort-myrmidon` default palette color.)pydoc");


}
