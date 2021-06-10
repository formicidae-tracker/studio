#include <pybind11/stl.h>

#include "BindMethods.hpp"

#include <fort/myrmidon/Types.hpp>
#include <fort/time/Time.hpp>

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
	py::class_<IdentifiedFrame,std::shared_ptr<IdentifiedFrame> >(m,"IdentifiedFrame")
		.def_readonly("FrameTime",&IdentifiedFrame::FrameTime,"The Time of the frame")
		.def_readonly("Space",&IdentifiedFrame::Space,"The space the frame belongs to")
		.def_readonly("Height",&IdentifiedFrame::Height,"The height of the tracking image")
		.def_readonly("Width",&IdentifiedFrame::Width,"The width of the tracking image")
		.def_property_readonly("Positions",
		                       []( const IdentifiedFrame & f ) -> const IdentifiedFrame::PositionMatrix & {
			                       return f.Positions;
		                       },py::return_value_policy::reference_internal,
		                       R"pydoc(A matrix with the ant positions.

)pydoc"
		                       )
		.def("Contains",&IdentifiedFrame::Contains)
		.def("At",&IdentifiedFrame::At)
		;
}

void BindCollisionFrame(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<Collision>(m,"Collision")
		.def_readonly("IDs",&Collision::IDs,"The AntID of the two Ants colliding")
		.def_readonly("Zone",&Collision::Zone,"The ZoneID where the collision happens")
		.def_property_readonly("Types",
		                       [](const Collision & c) -> const InteractionTypes & {
			                       return c.Types;
		                       },
		                       py::return_value_policy::reference_internal,
		                       "The matrix of AntShapeTypeID colliding")
		;
	py::class_<CollisionFrame,std::shared_ptr<CollisionFrame> >(m,"CollisionFrame")
		.def_readonly("FrameTime",&CollisionFrame::FrameTime,"The Time of the frame")
		.def_readonly("Space",&CollisionFrame::Space,"the Space the frame belongs to")
		.def_readonly("Collisions",&CollisionFrame::Collisions,"the list of Collision in the frame")
		;
}

void BindAntTrajectory(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<AntTrajectory,std::shared_ptr<AntTrajectory> >(m,"AntTrajectory")
		.def_readonly("Ant",&AntTrajectory::Ant,"The AntID of the Ant this trajectory describes.")
		.def_readonly("Space",&AntTrajectory::Space,"The SpaceID where the trajectory happens.")
		.def_readonly("Start",&AntTrajectory::Start,"The starting time of this trajectory.")
		.def_property_readonly("Positions",
		                       [](const AntTrajectory & t) -> const Eigen::Matrix<double,Eigen::Dynamic,5> & {
			                       return t.Positions;
		                       },
		                       py::return_value_policy::reference_internal,
		                       "The matrices of the ant pose in the trajectory local time")
		;

}

void BindAntInteraction(py::module_ & m) {
	using namespace fort::myrmidon;

	py::class_<AntTrajectorySegment>(m,"AntTrajectorySegment")
		.def_readonly("Trajectory",&AntTrajectorySegment::Trajectory)
		.def_readonly("Begin",&AntTrajectorySegment::Begin)
		.def_readonly("End",&AntTrajectorySegment::End)
		.def_property_readonly("Mean",
		                       [](const AntTrajectorySegment & ts) -> const Eigen::Vector3d & {
			                       if ( !ts.Mean) {
				                       throw std::runtime_error("py_fort_myrmidon.AntTrajectorySegment.Mean is not computed");
			                       }
			                       return *ts.Mean;
		                       },
		                       py::return_value_policy::reference_internal)
		;

	py::class_<AntInteraction,std::shared_ptr<AntInteraction>>(m,"AntInteraction")
		.def_readonly("IDs",&AntInteraction::IDs)
		.def_property_readonly("Types",
		                       [](const AntInteraction & i) -> const InteractionTypes & {
			                       return i.Types;
		                       },
		                       py::return_value_policy::reference_internal)
		.def_readonly("Trajectories",&AntInteraction::Trajectories)
		.def_readonly("Start",&AntInteraction::Start)
		.def_readonly("End",&AntInteraction::End)
		.def_readonly("Space",&AntInteraction::Space)
		;
}

void BindExperimentDataInfo(py::module_ & m) {
	using namespace fort::myrmidon;

	py::class_<TrackingDataDirectoryInfo>(m,"TrackingDataDirectoryInfo")
		.def_readonly("URI",&TrackingDataDirectoryInfo::URI)
		.def_readonly("AbsoluteFilePath",&TrackingDataDirectoryInfo::AbsoluteFilePath)
		.def_readonly("Frames",&TrackingDataDirectoryInfo::Frames)
		.def_readonly("Start",&TrackingDataDirectoryInfo::Start)
		.def_readonly("End",&TrackingDataDirectoryInfo::End)
		;

	py::class_<SpaceDataInfo>(m,"SpaceDataInfo")
		.def_readonly("URI",&SpaceDataInfo::URI)
		.def_readonly("Name",&SpaceDataInfo::Name)
		.def_readonly("Frames",&SpaceDataInfo::Frames)
		.def_readonly("Start",&SpaceDataInfo::Start)
		.def_readonly("End",&SpaceDataInfo::End)
		.def_readonly("TrackingDataDirectories",&SpaceDataInfo::TrackingDataDirectories)
		;

		py::class_<ExperimentDataInfo>(m,"ExperimentDataInfo")
		.def_readonly("Frames",&ExperimentDataInfo::Frames)
		.def_readonly("Start",&ExperimentDataInfo::Start)
		.def_readonly("End",&ExperimentDataInfo::End)
		.def_readonly("Spaces",&ExperimentDataInfo::Spaces)
		;
}

void BindTypes(py::module_ & m) {
	BindColor(m);
	BindTime(m);
	BindAntStaticValue(m);
	BindComputedMeasurement(m);
	BindIdentifiedFrame(m);
	BindCollisionFrame(m);
	BindAntTrajectory(m);
	BindAntInteraction(m);
	BindExperimentDataInfo(m);
}
