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

void BindComputedMeasurement(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<ComputedMeasurement>(m,"ComputedMeasurement")
		.def_readonly("Time",&ComputedMeasurement::Time,"The Time this measurement was made")
		.def_readonly("LengthMM",&ComputedMeasurement::LengthMM,"The length of the measurement in MM")
		.def_readonly("LengthPixel",&ComputedMeasurement::LengthPixel,"The length of the measurement in pixels")
		;
}

void BindTagStatistics(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<TagStatistics>(m,"TagStatistics")
		.def_readonly("TagID",&TagStatistics::ID,"The TagID it refers to")
		.def_readonly("FirstSeen",&TagStatistics::FirstSeen,"First time the tag was seen")
		.def_readonly("LastSeen",&TagStatistics::LastSeen,"Last time the tag was seen")
		.def_property_readonly("Counts",
		                       [](const TagStatistics & ts) -> const TagStatistics::CountVector & {
			                       return ts.Counts;
		                       },
		                       py::return_value_policy::reference_internal,
		                       "Histogram of gaps of non-detection of this tag.")
		;
}

void BindIdentifiedFrame(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<IdentifiedFrame,IdentifiedFrame::ConstPtr>(m,"IdentifiedFrame")
		.def_readonly("FrameTime",&IdentifiedFrame::FrameTime,"The time of the frame")
		.def_readonly("Space",&IdentifiedFrame::Space,"The space the frame belongs to")
		.def_readonly("Height",&IdentifiedFrame::Height,"The height of the tracking image")
		.def_readonly("Width",&IdentifiedFrame::Width,"The width of the tracking image")
		.def_property_readonly("Positions",
		                       []( const IdentifiedFrame & f ) -> const IdentifiedFrame::PositionMatrix & {
			                       return f.Positions;
		                       },py_return_value_polycy::reference_internal,
		                       R"pydoc(A matrix with the ant positions.

)pydoc"
		                       )
		.def("Contains",&IdentifiedFrame::Contains)
		.def("At",&IdentifiedFrame::At)

		;
}

void BindTypes(py::module_ & m) {
	BindTime(m);
	BindAntStaticValue(m);
	BindComputedMeasurement(m);
	BindIdentifiedFrame(m)
}
