#include <pybind11/stl.h>
#include <pybind11/eigen.h>

#include "BindMethods.hpp"

#include <fort/myrmidon/Types.hpp>
#include <fort/time/Time.hpp>

namespace py = pybind11;

template<class> inline constexpr bool always_false_v = false;


void BindComputedMeasurement(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<ComputedMeasurement>(m,
	                                "ComputedMeasurement",
	                                R"pydoc(
    A manual `fort-studio` measurement and its estimated value in
    millimeters.
)pydoc")
		.def_readonly("Time",
		              &ComputedMeasurement::Time,
		              "(py_fort_myrmidon.Time): the Time of the close-up this measurement.")
		.def_readonly("LengthMM",
		              &ComputedMeasurement::LengthMM,
		              "(float): its length in millimeters."
		              )
		.def_readonly("LengthPixel",
		              &ComputedMeasurement::LengthPixel,
		              "(float): its length in pixel.")
		;
}

void BindTagStatistics(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<TagStatistics>(m,
	                          "TagStatistics",
	                          "Tag detection statistics for a given TagID")
		.def_readonly("TagID",
		              &TagStatistics::ID,
		              "(int): the TagID it refers to")
		.def_readonly("FirstSeen",
		              &TagStatistics::FirstSeen,
		              "(py_fort_myrmidon.Time): first time the tag was seen")
		.def_readonly("LastSeen",
		              &TagStatistics::LastSeen,
		              "(py_fort_myrmidon.Time): last time the tag was seen")
		.def_property_readonly("Counts",
		                       [](const TagStatistics & ts) -> const TagStatistics::CountVector & {
			                       return ts.Counts;
		                       },
		                       py::return_value_policy::reference_internal,
		                       "(numpy.ndarray): histogram of gaps of non-detection of this tag.")
		;

}

void BindIdentifiedFrame(py::module_ & m) {
	using namespace fort::myrmidon;
	py::class_<IdentifiedFrame,std::shared_ptr<IdentifiedFrame> >(m,
	                                                              "IdentifiedFrame",
	                                                              R"pydoc(
    An IdentifiedFrame holds ant detection information associated with
one video frame.
)pydoc")
		.def_readonly("FrameTime",
		              &IdentifiedFrame::FrameTime,
		              "(py_fort_myrmidon.time): acquisition time of the frame")
		.def_readonly("Space",
		              &IdentifiedFrame::Space,
		              "(int): the SpaceID of the Space this frame comes from")
		.def_readonly("Height",
		              &IdentifiedFrame::Height,
		              "(int): height in pixel of the original video frame")
		.def_readonly("Width",
		              &IdentifiedFrame::Width,
		              "(int): width in pixel of the original video frame")
		.def_property_readonly("Positions",
		                       []( const IdentifiedFrame & f ) -> const IdentifiedFrame::PositionMatrix & {
			                       return f.Positions;
		                       },py::return_value_policy::reference_internal,
		                       R"pydoc(
( numpy.ndarray(numpy.float64[N,5]) ): an array of ant IDs, positions, orientations and current zones.
   * first column: the ant ID
   * second and third columns: xy coordinate of the ant, in [0;Width[ x [0;Height[ space.
   * fourth column: orientation of the ant in radian
   * fifth column: the current ZoneID. `0` if the Zone are not computed or if the ant is not in a zone.
 )pydoc")
		.def("Contains",
		     &IdentifiedFrame::Contains,
		     py::arg("antID"),
		     R"pydoc(
   Tests if the frame contains a given antID

   Args:
       antID (int): the AntID to test for.
   Returns:
       bool: `true` if antID is present in this IdentifiedFrame
)pydoc")
		.def("At",
		     &IdentifiedFrame::At,
		     py::arg("index"),
		     R"pydoc(
   Returns ant information for a given row.

   Args:
       index (int): the index in Positions
   Returns:
       Tuple[int,numpy.ndarray(numpy.float64[3,1]),int] : the AntID, a vector with its (x,y,theta) position, and its current zone.
   Raises:
       IndexError: if index >= len(Positions)
)pydoc")
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
	BindTagStatistics(m);
	BindComputedMeasurement(m);
	BindIdentifiedFrame(m);
	BindCollisionFrame(m);
	BindAntTrajectory(m);
	BindAntInteraction(m);
	BindExperimentDataInfo(m);
}
