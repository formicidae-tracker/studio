#include "Defer.hpp"

#ifndef NDEBUG
std::mutex Defer::PerfLock;
#endif //NDEBUG
