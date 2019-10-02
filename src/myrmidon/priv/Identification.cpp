#include "Identification.hpp"
#include "Ant.hpp"
#include "DeletedReference.hpp"

using namespace fort::myrmidon::priv;


Identification::Identification(const IdentifierPtr & identifier, const AntPtr & target)
	: d_target(target)
	, d_identifier(identifier) {
}


void Identification::Encode(fort::myrmidon::pb::Identification & pb) const {
	pb.Clear();
	if ( d_start ) {
		d_start->Encode(*pb.mutable_startframe());
	}
	if ( d_end ) {
		d_end->Encode(*pb.mutable_endframe());
	}
	pb.set_x(d_position.x());
	pb.set_y(d_position.y());
	pb.set_theta(d_position.z());
	pb.set_id(d_tagValue);
}

Identification::Ptr Identification::FromSaved(const fort::myrmidon::pb::Identification & pb,
                                              const IdentifierPtr & identifier,
                                              const Ant::Ptr & target) {
	std::shared_ptr<Identification> res(new Identification(identifier,target));
	if ( pb.has_startframe() ) {
		res->d_start = FramePointer::FromSaved(pb.startframe());
	}
	if ( pb.has_endframe() ) {
		res->d_end = FramePointer::FromSaved(pb.endframe());
	}
	res->d_position <<
		pb.x(),
		pb.y(),
		pb.theta();

	res->d_tagValue = pb.id();

	return res;
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

	if ( (end-begin) < 2 ) {
		return std::make_pair(List::const_iterator(),List::const_iterator());
	}

	for ( auto i = begin + 1;
	      i != end;
	      ++i) {
		auto prev = i-1;
		if ( !((*i)->d_start) || !((*prev)->d_end) || !((*prev)->d_end < (*i)->d_start) ) {
			return std::make_pair(prev,i);
		}
	}
	return std::make_pair(List::const_iterator(),List::const_iterator());
}


const FramePointer::Ptr & Identification::Start() const {
	return d_start;
}

const FramePointer::Ptr & Identification::End() const {
	return d_end;
}

Eigen::Vector2d Identification::TagPosition() const {
	return d_position.block<2,1>(0,0);
}

double Identification::TagAngle() const {
	return d_position.z();
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
