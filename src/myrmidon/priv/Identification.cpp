#include "Identification.hpp"
#include "Ant.hpp"
#include "DeletedReference.hpp"
#include "Identifier.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

const double Identification::DEFAULT_TAG_SIZE = 0.0;

Identification::Identification(TagID tagValue,
                               const IdentifierPtr & identifier,
                               const AntPtr & target)
	: d_antToTag(0.0,Eigen::Vector2d::Zero())
	, d_tagValue(tagValue)
	, d_target(target)
	, d_identifier(identifier)
	, d_tagSize(DEFAULT_TAG_SIZE) {
}

Time::ConstPtr Identification::Start() const {
	return d_start;
}

Time::ConstPtr Identification::End() const {
	return d_end;
}

Eigen::Vector2d Identification::AntPosition() const {
	return d_antToTag.translation();
}

double Identification::AntAngle() const {
	return d_antToTag.angle();
}

TagID Identification::TagValue() const {
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


Identification::Ptr Identification::Accessor::Create(TagID tagValue,
                                                     const IdentifierPtr & identifier,
                                                     const AntPtr & ant) {
	return std::shared_ptr<Identification>(new Identification(tagValue,identifier,ant));
}

void Identification::Accessor::SetStart(Identification & identification,
                                        const Time::ConstPtr & start) {
	identification.d_start = start;
}

void Identification::Accessor::SetEnd(Identification & identification,
                                      const Time::ConstPtr & end) {
	identification.d_end = end;
}


void Identification::SetAntPosition(const Eigen::Vector2d & position, double angle) {
	d_antToTag = Isometry2Dd(angle,position);
}


void Identification::SetBound(const Time::ConstPtr & start,
                              const Time::ConstPtr & end) {
	Time::ConstPtr oldStart(d_start),oldEnd(d_end);

	d_start = start;
	d_end = end;
	auto identifier = ParentIdentifier();
	try {
		List & tagSiblings = Identifier::Accessor::IdentificationsForTag(*identifier,d_tagValue);
		List & antSiblings = Ant::Accessor::Identifications(*Target());
		Identifier::SortAndCheck(tagSiblings,antSiblings);
	} catch ( const std::exception & e) {
		d_start = oldStart;
		d_end = oldEnd;
		throw;
	}
	Identifier::Accessor::UpdateIdentificationAntPosition(*identifier,this);
}

void Identification::SetStart(const Time::ConstPtr & start) {
	SetBound(start,d_end);
}

void Identification::SetEnd(const Time::ConstPtr & end) {
	SetBound(d_start,end);
}


void Identification::SetTagSize(double size) {
	d_tagSize = size;
}

double Identification::TagSize() const {
	return d_tagSize;
}

bool Identification::UseDefaultTagSize() const {
	return d_tagSize == DEFAULT_TAG_SIZE;
}

void Identification::ComputePositionFromTag(Eigen::Vector2d & position,
                                            double & angle,
                                            const Eigen::Vector2d & tagPosition,
                                            double tagAngle) const {
	Isometry2Dd tagToOrig(tagAngle,tagPosition);
	auto antToOrig = tagToOrig * d_antToTag;
	position = antToOrig.translation();
	angle = antToOrig.angle();
}


} // namespace priv
} // namespace myrmidon
} // namespace fort


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
