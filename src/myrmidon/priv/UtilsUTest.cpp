#include "UtilsUTest.hpp"

#include <myrmidon/UtilsUTest.hpp>


::testing::AssertionResult PolygonEqual(const fort::myrmidon::priv::Polygon::ConstPtr &a,
                                      const fort::myrmidon::priv::Polygon::ConstPtr &b) {
	if ( a->Size() != b->Size() ) {
		return ::testing::AssertionFailure() << "Polygon a and b have a different number of vertices a:"
		                                     << a->Size()
		                                     << " b:"
		                                     << b->Size();
	}
	for ( size_t i = 0; i < a->Size(); ++i ) {
		auto inter = VectorAlmostEqual(a->Vertex(i),b->Vertex(i));
		if ( !inter ) {
			return inter << "Vertex " << i << " differs";
		}
	}
	return ::testing::AssertionSuccess();
}


testing::AssertionResult CapsuleEqual(const fort::myrmidon::priv::Capsule::ConstPtr &a,
                                      const fort::myrmidon::priv::Capsule::ConstPtr &b) {
	auto inter = VectorAlmostEqual(a->C1(),b->C1());
	if ( !inter ) {
		return inter << "Center 1 differs";
	}
	inter = VectorAlmostEqual(a->C2(),b->C2());
	if ( !inter ) {
		return inter << "Center 2 differs";
	}
	inter = ::testing::internal::CmpHelperFloatingPointEQ<double>("a.C1.radius","a.C1.radius",a->R1(),b->R1());
	if ( !inter ) {
		return inter;
	}
	return ::testing::internal::CmpHelperFloatingPointEQ<double>("a.C2.radius","b.C2.radius",a->R2(),b->R2());
}


testing::AssertionResult CircleEqual(const fort::myrmidon::priv::Circle::ConstPtr &a,
                                       const fort::myrmidon::priv::Circle::ConstPtr &b) {
	auto inter = VectorAlmostEqual(a->Center(),b->Center());
	if ( !inter ) {
		return inter << "Center differs";
	}
	return ::testing::internal::CmpHelperFloatingPointEQ<double>("a.Radius","b.Radius",a->Radius(),b->Radius());
}



testing::AssertionResult ShapeEqual(const fort::myrmidon::priv::Shape::ConstPtr &a,
                                    const fort::myrmidon::priv::Shape::ConstPtr &b) {
	if ( a->ShapeType() != b->ShapeType() ) {
		return ::testing::AssertionFailure() << "Shape types differs";
	}

	switch(a->ShapeType()) {
	case fort::myrmidon::priv::Shape::Type::Capsule: {
		auto aa = fort::myrmidon::priv::Shape::ToCapsule(a);
		auto bb = fort::myrmidon::priv::Shape::ToCapsule(b);
		if ( !aa || !bb ) {
			return ::testing::AssertionFailure() << "Could not convert to Capsule";
		}
		return CapsuleEqual(aa,bb);

	}
	case fort::myrmidon::priv::Shape::Type::Circle: {
		auto aa = fort::myrmidon::priv::Shape::ToCircle(a);
		auto bb = fort::myrmidon::priv::Shape::ToCircle(b);
		if ( !aa || !bb ) {
			return ::testing::AssertionFailure() << "Could not convert to Circle";
		}
		return CircleEqual(aa,bb);
	}
	case fort::myrmidon::priv::Shape::Type::Polygon: {
		auto aa = fort::myrmidon::priv::Shape::ToPolygon(a);
		auto bb = fort::myrmidon::priv::Shape::ToPolygon(b);
		if ( !aa || !bb ) {
			return ::testing::AssertionFailure() << "Could not convert to Polygon";
		}
		return PolygonEqual(aa,bb);
	}
	}
	return ::testing::AssertionFailure() << "unsupported shape type";
}
