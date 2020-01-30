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
		                                     << " and a.Timestamp() and b.Timestamp() yied different results";
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

::testing::AssertionResult TimePtrEqual(const fort::myrmidon::Time::ConstPtr & a,
                                         const fort::myrmidon::Time::ConstPtr & b) {
	if (!a && !b) {
		return ::testing::AssertionSuccess();
	}

	if ( (!a && b) || (!b && a) ) {
		return ::testing::AssertionFailure() << "a: " << a << " != b: " << b;
	}
	return TimeEqual(*a,*b);
}
