#include "FramePointer.hpp"

#include <sstream>

#include "../utils/NotYetImplemented.hpp"

namespace fm = fort::myrmidon;

using namespace fort::myrmidon::priv;

uint64_t FramePointer::Frame() const {
	return d_frame;
}

const fs::path & FramePointer::Basepath() const {
	return d_path;
}


fs::path FramePointer::FullPath() const {
	std::ostringstream os;
	os << d_frame;
	return d_path / os.str();
}


const fm::Time & FramePointer::Time() const {
	return d_time;
}


FramePointer::ConstPtr FramePointer::Create(const fs::path & path, uint64_t frame, const fort::myrmidon::Time & t) {
	return std::shared_ptr<const FramePointer>(new FramePointer(path,frame,t));
}

FramePointer::FramePointer(const fs::path & path, uint64_t frame, const fort::myrmidon::Time & t)
	: d_path(path)
	, d_frame(frame)
	, d_time(t) {
}


std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::FramePointer & p) {
	out << p.Basepath().generic_string() << "/" << p.Frame();
}
