#include "Identification.hpp"
#include "Ant.hpp"
#include "DeletedReference.hpp"
#include "Identifier.hpp"

using namespace fort::myrmidon::priv;


Identification::Identification(uint32_t tagValue,
                               const IdentifierPtr & identifier,
                               const AntPtr & target)
	: d_tagValue(tagValue)
	, d_target(target)
	, d_identifier(identifier) {
}



std::pair<Identification::List::const_iterator,Identification::List::const_iterator>
Identification::SortAndCheckOverlap(Identification::List::iterator begin,
                                    Identification::List::iterator end) {

	std::sort(begin,
	          end,
	          [](const Ptr & a,
	             const Ptr & b) -> bool {
		          if ( !a->d_start ) {
			          return true;
		          }
		          if ( !b->d_start ) {
			          return false;
		          }
		          return *(a->d_start) < *(b->d_start);
	          });

	if ( std::distance(begin,end) < 2 ) {
		return std::make_pair(List::const_iterator(),List::const_iterator());
	}

	for ( auto i = begin + 1;
	      i != end;
	      ++i) {
		auto prev = i-1;
		if ( !((*i)->d_start) || !((*prev)->d_end) || !(*((*prev)->d_end) < *((*i)->d_start)) ) {
			return std::make_pair(prev,i);
		}
	}
	return std::make_pair(List::const_iterator(),List::const_iterator());
}


FramePointer::ConstPtr Identification::Start() const {
	return d_start;
}

FramePointer::ConstPtr Identification::End() const {
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
                                        const FramePointer::Ptr & start) {
	identification.d_start = start;
}

void Identification::Accessor::SetEnd(Identification & identification,
                                      const FramePointer::Ptr & end) {
	identification.d_end = end;
}


void Identification::SetTagPosition(const Eigen::Vector2d & position, double angle) {
	d_antToTag = Isometry2Dd(angle,position).inverse();
}


void Identification::SetBound(const FramePointer::Ptr & start,
                              const FramePointer::Ptr & end) {
	FramePointer::Ptr oldStart(d_start),oldEnd(d_end);

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

void Identification::SetStart(const FramePointer::Ptr & start) {
	SetBound(start,d_end);
}

void Identification::SetEnd(const FramePointer::Ptr & end) {
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
