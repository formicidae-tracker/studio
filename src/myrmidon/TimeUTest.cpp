#include "TimeUTest.hpp"

#include "Time.hpp"

using namespace fort::myrmidon;


TEST_F(TimeUTest,DurationCast) {
	Duration b(std::chrono::hours(1));

	EXPECT_EQ(b.Hours(),1.0);
	EXPECT_EQ(b.Minutes(),60.0);
	EXPECT_EQ(b.Seconds(),3600.0);
	EXPECT_EQ(b.Milliseconds(),3.6e6);
	EXPECT_EQ(b.Microseconds(),3.6e9);
	EXPECT_EQ(b.Nanoseconds(),3.6e12);

}


TEST_F(TimeUTest,DurationParsing) {
	//dataset taken from golang sources

	struct TestData {
		std::string Input;
		bool OK;
		Duration Expected;
	};

	std::vector<TestData> data
		= {
		   {"0",true,0},
		   {"5s", true, 5 * Second},
		   {"30s", true, 30 * Second},
		   {"1478s", true, 1478 * Second},
		   // sign
		   {"-5s", true, -5 * Second},
		   {"+5s", true, 5 * Second},
		   {"-0", true, 0},
		   {"+0", true, 0},
		   // decimal
		   {"5.0s", true, 5 * Second},
		   {"5.6s", true, 5*Second + 600*Millisecond},
		   {"5.s", true, 5 * Second},
		   {".5s", true, 500 * Millisecond},
		   {"1.0s", true, 1 * Second},
		   {"1.00s", true, 1 * Second},
		   {"1.004s", true, 1*Second + 4*Millisecond},
		   {"1.0040s", true, 1*Second + 4*Millisecond},
		   {"100.00100s", true, 100*Second + 1*Millisecond},
		   // different units
		   {"10ns", true, 10 * Nanosecond},
		   {"11us", true, 11 * Microsecond},
		   {"12µs", true, 12 * Microsecond}, // U+00B5
		   {"12μs", true, 12 * Microsecond}, // U+03BC
		   {"13ms", true, 13 * Millisecond},
		   {"14s", true, 14 * Second},
		   {"15m", true, 15 * Minute},
		   {"16h", true, 16 * Hour},
		   // composite durations
		   {"3h30m", true, 3*Hour + 30*Minute},
		   {"10.5s4m", true, 4*Minute + 10*Second + 500*Millisecond},
		   {"-2m3.4s", true, -(2*Minute + 3*Second + 400*Millisecond)},
		   {"1h2m3s4ms5us6ns", true, 1*Hour + 2*Minute + 3*Second + 4*Millisecond + 5*Microsecond + 6*Nanosecond},
		   {"39h9m14.425s", true, 39*Hour + 9*Minute + 14*Second + 425*Millisecond},
		   // large value
		   {"52763797000ns", true, 52763797000 * Nanosecond},
		   // more than 9 digits after decimal point, see https://golang.org/issue/6617
		   {"0.3333333333333333333h", true, 20 * Minute},
		   // 9007199254740993 = 1<<53+1 cannot be stored precisely in a float64
		   {"9007199254740993ns", true, ( (int64_t(1)<<53) + int64_t(1) ) * Nanosecond },
		   // largest duration that can be represented by int64 in nanoseconds
		   {"9223372036854775807ns", true, std::numeric_limits<int64_t>::max() },
		   {"9223372036854775.807us", true, std::numeric_limits<int64_t>::max() },
		   {"9223372036s854ms775us807ns", true, std::numeric_limits<int64_t>::max() },
		   // large negative value
		   {"-9223372036854775807ns", true, (-1L<<63) + 1*Nanosecond},
		   // huge string; issue 15011.
		   {"0.100000000000000000000h", true, 6 * Minute},
		   // This value tests the first overflow check in leadingFraction.
		   {"0.830103483285477580700h", true, 49*Minute + 48*Second + 372539827*Nanosecond},

		   	// errors
		   	{"", false, 0},
		   	{"3", false, 0},
		   	{"-", false, 0},
		   	{"s", false, 0},
		   	{".", false, 0},
		   	{"-.", false, 0},
		   	{".s", false, 0},
		   	{"+.s", false, 0},
		   	{"3000000h", false, 0},                  // overflow
		   	{"9223372036854775808ns", false, 0},     // overflow
		   	{"9223372036854775.808us", false, 0},    // overflow
		   	{"9223372036854ms775us808ns", false, 0}, // overflow
		   	// largest negative value of type int64 in nanoseconds should fail
		   	// see https://go-review.googlesource.com/#/c/2461/
		   	{"-9223372036854775808ns", false, 0},
	};

	for ( const auto & d : data) {

		if ( d.OK == true ) {
			try {
				auto res = Duration::Parse(d.Input);
				EXPECT_EQ(res.Nanoseconds(),d.Expected.Nanoseconds()) << "parsing '" << d.Input << "'";
			} catch (const std::exception & e ) {
				ADD_FAILURE() << "Unexpected exception while parsing '" << d.Input
				              << "': " << e.what();
			}
		} else {
			try {
				auto res = Duration::Parse(d.Input);
				ADD_FAILURE() << "Should have received an exception while parsing '" << d.Input << "' but got result: " << res.Nanoseconds();
			} catch ( const std::exception & e) {
			}
		}
	}
}
