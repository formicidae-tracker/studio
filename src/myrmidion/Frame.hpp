#pragma once


#include <memory>
#include "Ant.hpp"

namespace fort {

namespace myrmidion {


class Frame {
public:
	typedef uint64_t ID;
	typedef uint64_t Timestamp;
	typedef std::shared_ptr<Frame> Ptr;

private:
	Timestamp d_timestamp;
	ID d_ID;

	Ant::List d_ants;
};

} //namespace myrmidion

} //namespace fort
