#include <pybind11/stl.h>

#include "BindMethods.hpp"

#include <fort/myrmidon/Types.hpp>

namespace py = pybind11;

template<class> inline constexpr bool always_false_v = false;

void BindAntStaticValue(py::module_ & m) {

	py::class_<fort::myrmidon::AntStaticValue>(m,"AntStaticValue")
		.def_static("Type",
		            [](const fort::myrmidon::AntStaticValue & v) -> std::string {
			            return std::visit([](auto&& arg) {
				                              using T = std::decay_t<decltype(arg)>;
				                              if constexpr (std::is_same_v<T, bool>)
					                                           return "bool";
				                              else if constexpr (std::is_same_v<T, int>)
					                                                return "int";
				                              else if constexpr (std::is_same_v<T, double>)
					                                                return "float";
				                              else if constexpr (std::is_same_v<T, std::string>)
					                                                return "string";
				                              else if constexpr (std::is_same_v<T, fort::Time>)
					                                                return "py_fort_myrmidon.Time";
				                              else
					                              static_assert(always_false_v<T>, "non-exhaustive visitor!");
			                              }, v);
	      },"prints an AntStaticValue type as seen as C++ side (for unit testing)")
		.def_static("TrueValue",[](){ return fort::myrmidon::AntStaticValue(true);},"C++ emitted boolean value (for unit testing)")
		.def_static("IntValue",[](){ return fort::myrmidon::AntStaticValue(123);},"C++ emitted int value (for unit testing)")
		.def_static("FloatValue",[](){ return fort::myrmidon::AntStaticValue(123.123);},"C++ emitted float value (for unit testing)")
		.def_static("StringValue",[](){ return fort::myrmidon::AntStaticValue(std::string("string"));},"C++ emitted string value (for unit testing)")
		.def_static("TimeValue",[](){ return fort::myrmidon::AntStaticValue(fort::Time());},"C++ emitted Time value (for unit testing)")
		;


}


void BindTypes(py::module_ & m) {
	BindTime(m);
	BindAntStaticValue(m);
}
