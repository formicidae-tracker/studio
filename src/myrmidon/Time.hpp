#pragma once

#include <cstdint>

namespace fort {

namespace myrmidon {

// A Duration represent time ellapsed between two Time

class Duration {
	// construct
	Duration(uint64_t ns);



private:
	uint64_t d_nanosecond;
};


} // namespace myrmidon

} // namespace fort
