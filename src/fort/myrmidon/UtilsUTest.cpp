#include "UtilsUTest.hpp"

#include <google/protobuf/util/message_differencer.h>

::testing::AssertionResult TimeEqual(const fort::myrmidon::Time & a,
                                     const fort::myrmidon::Time & b) {
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
