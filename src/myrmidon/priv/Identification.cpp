#include "Identification.hpp"
#include "Ant.hpp"
#include "DeletedReference.hpp"
#include "Identifier.hpp"

namespace fm = fort::myrmidon;
using namespace fort::myrmidon::priv;


Identification::Identification(uint32_t tagValue,
                               const IdentifierPtr & identifier,
                               const AntPtr & target)
	: d_tagValue(tagValue)
	, d_target(target)
	, d_identifier(identifier) {
}

fm::Time::ConstPtr Identification::Start() const {
	return d_start;
}

fm::Time::ConstPtr Identification::End() const {
	return d_end;
}

Eigen::Vector2d Identification::TagPosition() const {
	return d_antToTag.inverse().translation();
}

double Identification::TagAngle() const {
	return -d_antToTag.angle();
}

uint32_t Identification::TagValue() const {
	return d_tagValue;
}

Ant::Ptr Identification::Target() const {
	auto res = d_target.lock();
	if (!res) {
		throw DeletedReference<Ant>();
	}
	return res;
}


OverlappingIdentification::OverlappingIdentification(const Identification & a,
                                                          const Identification & b)
	: std::runtime_error(Reason(a,b)){
}

std::string OverlappingIdentification::Reason(const Identification & a,
                                              const Identification & b) {
	std::ostringstream os;
	os << a << " and " << b << " overlaps";
	return os.str();
}


Identifier::Ptr Identification::ParentIdentifier() const {
	auto res = d_identifier.lock();
	if (!res) {
		throw DeletedReference<Identifier>();
	}
	return res;
}


Identification::Ptr Identification::Accessor::Create(uint32_t tagValue,
                                                    const IdentifierPtr & identifier,
                                                    const AntPtr & ant) {
	return std::shared_ptr<Identification>(new Identification(tagValue,identifier,ant));
}

void Identification::Accessor::SetStart(Identification & identification,
                                        const fm::Time::ConstPtr & start) {
	identification.d_start = start;
}

void Identification::Accessor::SetEnd(Identification & identification,
                                      const Time::ConstPtr & end) {
	identification.d_end = end;
}


void Identification::SetTagPosition(const Eigen::Vector2d & position, double angle) {
	d_antToTag = Isometry2Dd(angle,position).inverse();
}


void Identification::SetBound(const Time::ConstPtr & start,
                              const Time::ConstPtr & end) {
	Time::ConstPtr oldStart(d_start),oldEnd(d_end);

	d_start = start;
	d_end = end;

	try {
		List & tagSiblings = Identifier::Accessor::IdentificationsForTag(*ParentIdentifier(),d_tagValue);
		List & antSiblings = Ant::Accessor::Identifications(*Target());
		Identifier::SortAndCheck(tagSiblings,antSiblings);
	} catch ( const std::exception & e) {
		d_start = oldStart;
		d_end = oldEnd;
		throw;
	}
}

void Identification::SetStart(const Time::ConstPtr & start) {
	SetBound(start,d_end);
}

void Identification::SetEnd(const Time::ConstPtr & end) {
	SetBound(d_start,end);
}


void Identification::ComputeTagToAntTransform(Isometry2Dd & result,
                                              const Eigen::Vector2d & tagPosition, double tagAngle,
                                              const Eigen::Vector2d & head,
                                              const Eigen::Vector2d & tail) {
	Eigen::Vector2d dir = head - tail;
	dir.normalize();

	result = Isometry2Dd(std::atan2(dir.y(),dir.x()),(head+tail)/2).inverse() * Isometry2Dd(tagAngle,tagPosition);
}


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::Identification & a) {
	out << "Identification{ID:"
	    << a.TagValue()
	    << "↦"
	    << a.Target()->ID()
	    << ", From:'";
	if (a.Start()) {
		out << a.Start()->DebugString();
	} else {
		out << "-∞";
	}
	out << "', To:'";
	if (a.End()) {
		out << a.End()->DebugString();
	} else {
		out << "+∞";
	}
	return out << "'}";
}
