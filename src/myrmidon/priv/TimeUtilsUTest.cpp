#include "TimeUtilsUTest.hpp"

#include "TimeUtils.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(TimeUtilsUTest,TimeConversion) {
	int timestampUS = 1234;
	hermes::FrameReadout allGood,hasError,hasNoTimestamp,hasErrorButTimestamp;
	//No error frame with timestamp gets a monotonic value
	allGood.set_timestamp(timestampUS);
	hasError.set_error(hermes::FrameReadout_Error_PROCESS_OVERFLOW);
	hasErrorButTimestamp.set_error(hermes::FrameReadout_Error_ILLUMINATION_ERROR);
	hasErrorButTimestamp.set_timestamp(timestampUS);


	auto allGoodTime = TimeFromFrameReadout(allGood,1);
	auto hasErrorTime = TimeFromFrameReadout(hasError,1);
	auto hasNoTimestampTime = TimeFromFrameReadout(hasNoTimestamp,1);
	auto hasErrorButTimestampTime = TimeFromFrameReadout(hasErrorButTimestamp,1);

	EXPECT_FALSE(hasErrorTime.HasMono());
	EXPECT_FALSE(hasNoTimestampTime.HasMono());
	EXPECT_FALSE(hasErrorButTimestampTime.HasMono());

	ASSERT_TRUE(allGoodTime.HasMono());

	//converts timestamp us to internal ns
	EXPECT_EQ(allGoodTime.MonotonicValue(),timestampUS * 1000);

}

} //namespace priv
} //namespace myrmidon
} //namespace fort
