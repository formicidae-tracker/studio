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
	, d_tagSize(DEFAULT_TAG_SIZE)
	, d_userDefinedPose(false) {
	d_start = Time::SinceEver();
	d_end = Time::Forever();
}

const Time & Identification::Start() const {
	return d_start;
}

const Time & Identification::End() const {
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
                                        const Time & start) {
	identification.d_start = start;
}

void Identification::Accessor::SetEnd(Identification & identification,
                                      const Time & end) {
	identification.d_end = end;
}

void Identification::Accessor::SetAntPosition(Identification & identification,
                                              const Eigen::Vector2d& position,
                                              double angle) {
	identification.SetAntPosition(position,angle);
}


void Identification::SetAntPosition(const Eigen::Vector2d & position, double angle) {
	d_antToTag = Isometry2Dd(angle,position);
}


void Identification::SetBound(const Time & start,
                              const Time & end) {
	Time oldStart(d_start),oldEnd(d_end);

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

void Identification::SetStart(const Time & start) {
	SetBound(start,d_end);
}

void Identification::SetEnd(const Time & end) {
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

void Identification::ComputePositionFromTag(Vector2dRef antPosition,
                                            double & antAngle,
                                            const Eigen::Vector2d & tagPosition,
                                            double tagAngle) const {
	Isometry2Dd tagToOrig(tagAngle,tagPosition);
	auto antToOrig = tagToOrig * d_antToTag;
	antPosition = antToOrig.translation();
	antAngle = antToOrig.angle();
}


void Identification::SetUserDefinedAntPose(const Eigen::Vector2d & antPosition, double antAngle) {
	d_userDefinedPose = true;
	SetAntPosition(antPosition,antAngle);
}

void Identification::ClearUserDefinedAntPose() {
	auto identifier = ParentIdentifier();
	d_userDefinedPose = false;
	Identifier::Accessor::UpdateIdentificationAntPosition(*identifier,this);
}


} // namespace priv
} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::Identification & a) {
	out << "Identification{ID:"
	    << fort::myrmidon::FormatTagID(a.TagValue())
	    << " ↦ "
	    << a.Target()->AntID()
	    << ", From:"
	    << a.Start()
	    << ", To:"
		<< a.End()
	    << "}";
	return out;
}
