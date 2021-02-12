#include <fort/myrmidon/Time.hpp>

#include "UtilsUTest.hpp"

#include <fort/myrmidon/UtilsUTest.hpp>


::testing::AssertionResult PolygonEqual(const fort::myrmidon::priv::Polygon &a,
                                      const fort::myrmidon::priv::Polygon &b) {
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


testing::AssertionResult CapsuleEqual(const fort::myrmidon::priv::Capsule &a,
                                      const fort::myrmidon::priv::Capsule &b) {
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


testing::AssertionResult CircleEqual(const fort::myrmidon::priv::Circle &a,
                                     const fort::myrmidon::priv::Circle &b) {
	auto inter = VectorAlmostEqual(a.Center(),b.Center());
	if ( !inter ) {
		return inter << "Center differs";
	}
	return ::testing::internal::CmpHelperFloatingPointEQ<double>("a.Radius","b.Radius",a.Radius(),b.Radius());
}



testing::AssertionResult ShapeEqual(const fort::myrmidon::priv::Shape::ConstPtr &a,
                                    const fort::myrmidon::priv::Shape::ConstPtr &b) {
	if ( a->ShapeType() != b->ShapeType() ) {
		return ::testing::AssertionFailure() << "Shape types differs";
	}

	switch(a->ShapeType()) {
	case fort::myrmidon::Shape::Type::Capsule: {
		auto aa = fort::myrmidon::priv::Shape::ToCapsule(a);
		auto bb = fort::myrmidon::priv::Shape::ToCapsule(b);
		if ( !aa || !bb ) {
			return ::testing::AssertionFailure() << "Could not convert to Capsule";
		}
		return CapsuleEqual(*aa,*bb);

	}
	case fort::myrmidon::Shape::Type::Circle: {
		auto aa = fort::myrmidon::priv::Shape::ToCircle(a);
		auto bb = fort::myrmidon::priv::Shape::ToCircle(b);
		if ( !aa || !bb ) {
			return ::testing::AssertionFailure() << "Could not convert to Circle";
		}
		return CircleEqual(*aa,*bb);
	}
	case fort::myrmidon::Shape::Type::Polygon: {
		auto aa = fort::myrmidon::priv::Shape::ToPolygon(a);
		auto bb = fort::myrmidon::priv::Shape::ToPolygon(b);
		if ( !aa || !bb ) {
			return ::testing::AssertionFailure() << "Could not convert to Polygon";
		}
		return PolygonEqual(*aa,*bb);
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
		if ( std::get<fort::myrmidon::Time>(a).Equals(std::get<fort::myrmidon::Time>(b)) == false ) {
			return ::testing::AssertionFailure() << "a:" <<  std::get<fort::myrmidon::Time>(a)
			                                     << " b:" << std::get<fort::myrmidon::Time>(b)
			                                     << " differs!";
		}
		break;
	default:
		return ::testing::AssertionFailure() << "Unknown type index: " << a.index();
	}
	return ::testing::AssertionSuccess();
}

::testing::AssertionResult
AABBAlmostEqual(const fort::myrmidon::priv::AABB & a,
                const fort::myrmidon::priv::AABB & b) {
	auto min = VectorAlmostEqual(a.min(),b.min());
	if ( !min == true ) {
		return min;
	}
	return VectorAlmostEqual(a.max(),b.max());
}
