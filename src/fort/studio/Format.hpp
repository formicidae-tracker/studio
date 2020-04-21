#pragma once

#include <fort/myrmidon/Time.hpp>

#include <QString>


template <typename T>
inline std::string ToString(const T & t) {
	std::ostringstream os;
	os << t;
	return os.str();
}

template <>
inline std::string ToString<bool>(const bool & t) {
	std::ostringstream os;
	os << std::boolalpha << t;
	return os.str();
}

template <class T>
std::string ToString(const std::shared_ptr<T> & ptr) {
	return ToString(*ptr);
};


template <typename T>
inline QString ToQString(const T & t) {
	return ToString(t).c_str();
}

template <>
QString ToQString<fort::myrmidon::Time::Ptr>(const fort::myrmidon::Time::Ptr &) = delete;

template <>
QString ToQString<fort::myrmidon::Time::ConstPtr>(const fort::myrmidon::Time::ConstPtr &) = delete;


inline std::string ToString(const fort::myrmidon::Time::ConstPtr & t, const std::string & prefix) {
	if ( !t ) {
		return prefix + "∞";
	}
	std::ostringstream os;
	os << t;
	return os.str();
}

inline QString ToQString(const fort::myrmidon::Time::ConstPtr & t, const std::string & prefix) {
	return ToString(t,prefix).c_str();
}

inline std::string ToStdString(const QString & s) {
	return s.toUtf8().constData();
}
