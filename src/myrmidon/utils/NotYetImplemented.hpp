#pragma once

#include <stdexcept>

class NotYetImplemented : public std::logic_error {
public:
	NotYetImplemented(const std::string & functionName)
		: std::logic_error(functionName + " is not yet implemented") {
	}
	virtual ~NotYetImplemented() {}
};


#define MYRMIDON_NOT_YET_IMPLEMENTED() NotYetImplemented(__PRETTY_FUNCTION__)
