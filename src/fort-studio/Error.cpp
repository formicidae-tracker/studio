#include "Error.hpp"

Error::Error(const char * what)
	: d_what(what) {}

Error::Error(const QString & what)
	: d_what(what) {}

Error::Error(const std::string & what)
	: d_what(what.c_str()) {
}

Error::~Error() {}

bool Error::OK() const {
	return d_what.isEmpty();
}

const QString & Error::what() const {
	return d_what;
}


bool Error::operator==(const Error & other ) const {
	return d_what == other.d_what;
}

const Error Error::NONE("");
