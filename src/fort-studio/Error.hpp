#pragma once

#include <QString>


class Error {
public:
	Error(const char * what);
	Error(const QString & what);
	~Error();

	bool OK() const;

	const QString & what() const;

	static const Error NONE;

	bool operator==(const Error & other ) const ;

	bool operator()() const {
		return OK();
	}
private:
	QString d_what;
};
