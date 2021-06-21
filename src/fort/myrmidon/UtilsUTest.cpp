#include "UtilsUTest.hpp"

#include <google/protobuf/util/message_differencer.h>

#include <Eigen/Geometry>

::testing::AssertionResult TimeEqual(const fort::Time & a,
                                     const fort::Time & b) {
	if ( a.Equals(b) == false ) {
		return ::testing::AssertionFailure() << "a: " << a.DebugString()
		                                     << "b: " << b.DebugString()
		                                     << " and a.Equals(b) returns false";
	}

	if ( google::protobuf::util::MessageDifferencer::Equals(a.ToTimestamp(),b.ToTimestamp()) == false ) {
		return ::testing::AssertionFailure() << "a: " << a.DebugString()
		                                     << "b: " << b.DebugString()
		                                     << " and a.Timestamp() and b.Timestamp() yield different results";
	}

	if ( a.HasMono() == false ) {
		if (b.HasMono() == true ) {
			return ::testing::AssertionFailure() << "a: " << a.DebugString()
			                                     << "b: " << b.DebugString()
			                                     << " a has no monotonic time but b has one";
		} else {
			return ::testing::AssertionSuccess();
		}
	}

	if ( b.HasMono() ==  false ) {
		return ::testing::AssertionFailure() << "a: " << a.DebugString()
		                                     << "b: " << b.DebugString()
		                                     << " a has monotonic time but b hasn't";
	}

	if ( a.MonoID() != b.MonoID() ) {
		return ::testing::AssertionFailure() << "a: " << a.DebugString()
		                                     << "b: " << b.DebugString()
		                                     << " a and b have different monotonic ID";
	}

	if ( a.MonotonicValue() != b.MonotonicValue() ) {
		return ::testing::AssertionFailure() << "a: " << a.DebugString()
		                                     << "b: " << b.DebugString()
		                                     << " a and b have different monotonic value";
	}

	return ::testing::AssertionSuccess();
}



::testing::AssertionResult VectorAlmostEqual(const Eigen::Vector2d & a,
                             const Eigen::Vector2d & b) {
	auto xAssertion = ::testing::internal::CmpHelperFloatingPointEQ<double>("a.x","b.x",a.x(),b.x());
	if ( xAssertion == false ) {
		return ::testing::AssertionFailure() << "a:{" << a.transpose()
		                                     << "} b: {" << b.transpose()
		                                     << "}: " << xAssertion.message();
	}
	auto yAssertion = ::testing::internal::CmpHelperFloatingPointEQ<double>("a.y","b.y",a.y(),b.y());
	if ( yAssertion == false ) {
		return ::testing::AssertionFailure() << "a:{" << a.transpose()
		                                     << "} b: {" << b.transpose()
		                                     << "}: " << yAssertion.message();

	}
	return ::testing::AssertionSuccess();
}


::testing::AssertionResult MessageEqual(const google::protobuf::Message &a,
                                        const google::protobuf::Message &b) {
	std::string differences;

	google::protobuf::util::MessageDifferencer diff;
	diff.set_float_comparison(google::protobuf::util::MessageDifferencer::APPROXIMATE);
	diff.ReportDifferencesToString(&differences);
	if ( diff.Compare(b,a) == true ) {
		return ::testing::AssertionSuccess();
	}
	return ::testing::AssertionFailure() << "a:{" << a.ShortDebugString()
	                                     << "}, b:{" << b.ShortDebugString()
	                                     << "} differs: " << differences;
}


::testing::AssertionResult PolygonEqual(const fort::myrmidon::Polygon &a,
                                        const fort::myrmidon::Polygon &b) {
	if ( a.Size() != b.Size() ) {
		return ::testing::AssertionFailure() << "Polygon a and b have a different number of vertices a:"
		                                     << a.Size()
		                                     << " b:"
		                                     << b.Size();
	}
	for ( size_t i = 0; i < a.Size(); ++i ) {
		auto inter = VectorAlmostEqual(a.Vertex(i),b.Vertex(i));
		if ( !inter ) {
			return inter << "Vertex " << i << " differs";
		}
	}
	return ::testing::AssertionSuccess();
}


testing::AssertionResult CapsuleEqual(const fort::myrmidon::Capsule &a,
                                      const fort::myrmidon::Capsule &b) {
	auto inter = VectorAlmostEqual(a.C1(),b.C1());
	if ( !inter ) {
		return inter << "Center 1 differs";
	}
	inter = VectorAlmostEqual(a.C2(),b.C2());
	if ( !inter ) {
		return inter << "Center 2 differs";
	}
	inter = ::testing::internal::CmpHelperFloatingPointEQ<double>("a.C1.radius","a.C1.radius",a.R1(),b.R1());
	if ( !inter ) {
		return inter;
	}
	return ::testing::internal::CmpHelperFloatingPointEQ<double>("a.C2.radius","b.C2.radius",a.R2(),b.R2());
}


testing::AssertionResult CircleEqual(const fort::myrmidon::Circle &a,
                                     const fort::myrmidon::Circle &b) {
	auto inter = VectorAlmostEqual(a.Center(),b.Center());
	if ( !inter ) {
		return inter << "Center differs";
	}
	return ::testing::internal::CmpHelperFloatingPointEQ<double>("a.Radius","b.Radius",a.Radius(),b.Radius());
}



testing::AssertionResult ShapeEqual(const fort::myrmidon::Shape & a,
                                    const fort::myrmidon::Shape & b) {
	if ( a.ShapeType() != b.ShapeType() ) {
		return ::testing::AssertionFailure() << "Shape types differs";
	}

	switch(a.ShapeType()) {
	case fort::myrmidon::Shape::Type::Capsule: {
		return CapsuleEqual(static_cast<const fort::myrmidon::Capsule &>(a),
		                    static_cast<const fort::myrmidon::Capsule &>(b));

	}
	case fort::myrmidon::Shape::Type::Circle: {
		return CircleEqual(static_cast<const fort::myrmidon::Circle &>(a),
		                   static_cast<const fort::myrmidon::Circle &>(b));
	}
	case fort::myrmidon::Shape::Type::Polygon: {
		return PolygonEqual(static_cast<const fort::myrmidon::Polygon &>(a),
		                    static_cast<const fort::myrmidon::Polygon &>(b));
	}
	}
	return ::testing::AssertionFailure() << "unsupported shape type";
}

::testing::AssertionResult AntStaticValueEqual(const fort::myrmidon::AntStaticValue &a,
                                               const fort::myrmidon::AntStaticValue &b) {
	if ( a.index() != b.index() ) {
		return ::testing::AssertionFailure() << "StaticValue Type differs a:" << a.index()
		                                     << " b:" <<b.index();
	}

	switch( a.index() ) {
	case 0:
		if ( std::get<bool>(a) != std::get<bool>(b) ) {
			return ::testing::AssertionFailure() << "a: " << std::boolalpha
			                                     << std::get<bool>(a)
			                                     << " b: " << std::get<bool>(b)
			                                     << " differs!";
		}
		break;
	case 1:
		if ( std::get<int>(a) != std::get<int>(b) ) {
			return ::testing::AssertionFailure() << "a:" <<  std::get<int>(a)
			                                     << " b:" << std::get<int>(b)
			                                     << " differs!";
		}
		break;
	case 2:
		if ( std::get<double>(a) != std::get<double>(b) ) {
			return ::testing::AssertionFailure() << "a:" <<  std::get<double>(a)
			                                     << " b:" << std::get<double>(b)
			                                     << " differs!";
		}
		break;
	case 3:
		if ( std::get<std::string>(a) != std::get<std::string>(b) ) {
			return ::testing::AssertionFailure() << "a:" <<  std::get<std::string>(a)
			                                     << " b:" << std::get<std::string>(b)
			                                     << " differs!";
		}
		break;
	case 4:
		if ( std::get<fort::Time>(a).Equals(std::get<fort::Time>(b)) == false ) {
			return ::testing::AssertionFailure() << "a:" <<  std::get<fort::Time>(a)
			                                     << " b:" << std::get<fort::Time>(b)
			                                     << " differs!";
		}
		break;
	default:
		return ::testing::AssertionFailure() << "Unknown type index: " << a.index();
	}
	return ::testing::AssertionSuccess();
}

::testing::AssertionResult
AABBAlmostEqual(const fort::myrmidon::AABB & a,
                const fort::myrmidon::AABB & b) {
	auto min = VectorAlmostEqual(a.min(),b.min());
	if ( !min == true ) {
		return min;
	}
	return VectorAlmostEqual(a.max(),b.max());
}
