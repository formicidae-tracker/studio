#include "Ant.hpp"

#include <sstream>
#include <iomanip>


using namespace fort::myrmidon::priv;

Ant::Ant(uint32_t ID)
	: d_ID(ID) {
	std::ostringstream os;
	os << "0x" << std::setw(4) << std::setfill('0') << std::hex
	   << std::uppercase << ID;
	d_IDStr = os.str();
}

Ant::~Ant() {
}


Ant::Identification::List & Ant::Identifications(){
	return d_identifications;
}

const Ant::Identification::List & Ant::ConstIdentifications() const {
	return d_identifications;
}

void Ant::SortAndCheckIdentifications() {
	auto firstOverlap = Identification::SortAndCheckOverlap(d_identifications.begin(),
	                                                        d_identifications.end());
	if ( firstOverlap.first != firstOverlap.second) {
		throw OverlappingIdentification(**(firstOverlap.first),**(firstOverlap.second));
	}
}


std::ostream & operator<<(std::ostream & out, const Ant::Identification & a) {
	out << "Identification{ID:"
	    << a.TagValue()
	    << ", From:'";
	if (a.Start()) {
		out << a.Start()->Path << "/" << a.Start()->Frame;
	} else {
		out << "<begin>";
	}
	out << "', To:'";
	if (a.End()) {
		out << a.End()->Path << "/" << a.End()->Frame;
	} else {
		out << "<end>";
	}
	return out << "'}";

}


Ant::OverlappingIdentification::OverlappingIdentification(const Identification & a,
                                                          const Identification & b)
	: std::runtime_error(Reason(a,b)){
}
std::string Ant::OverlappingIdentification::Reason(const Identification & a,
                                                   const Identification & b) {
	std::ostringstream os;
	os << a << " and " << b << " overlaps";
	return os.str();
}



Ant::Estimate::Estimate(const Eigen::Vector2d & head,
                        const Eigen::Vector2d & tail,
                        const Eigen::Vector3d & tagPosition,
                        const std::filesystem::path & fromFile)
	: d_fromFile(fromFile) {
	d_data.block<2,1>(0,0) = head;
	d_data.block<2,1>(2,0) = tail;
	d_data.block<3,1>(4,0) = tagPosition;
}

Eigen::Vector2d Ant::Estimate::Head() const {
	return d_data.block<2,1>(0,0);
}

Eigen::Vector2d Ant::Estimate::Tail() const {
	return d_data.block<2,1>(2,0);
}

Eigen::Vector3d Ant::Estimate::TagPosition() const {
	return d_data.block<3,1>(4,0);
}

Eigen::Vector3d Ant::Estimate::Invert() const {
	return Eigen::Vector3d::Zero();
}

std::filesystem::path Ant::Estimate::FromFile() const {
	return d_fromFile;
}

void Ant::Estimate::Encode(fort::myrmidon::pb::Estimate & pb) const {
	pb.set_fromfile(d_fromFile.generic_string());
	pb.set_xhead(d_data(0,0));
	pb.set_yhead(d_data(1,0));
	pb.set_xtail(d_data(2,0));
	pb.set_ytail(d_data(3,0));
	pb.set_xposition(d_data(4,0));
	pb.set_yposition(d_data(5,0));
	pb.set_thetaposition(d_data(6,0));
}


Ant::Estimate Ant::Estimate::FromSaved(const fort::myrmidon::pb::Estimate & pb) {
	Estimate res;
	res.d_fromFile = pb.fromfile();
	res.d_data <<
		pb.xhead(),
		pb.yhead(),
		pb.xtail(),
		pb.ytail(),
		pb.xposition(),
		pb.yposition(),
		pb.thetaposition();
	return res;
}



void Ant::Identification::Encode(fort::myrmidon::pb::Identification & pb) const {
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

Ant::Identification::Ptr Ant::Identification::FromSaved(const fort::myrmidon::pb::Identification & pb, const Ant::Ptr & target) {
	std::shared_ptr<Identification> res(new Identification(target));
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

void Ant::Encode(fort::myrmidon::pb::AntMetadata & pb) const {
	pb.Clear();
	pb.set_id(d_ID);
	for ( const auto & i : d_identifications ) {
		i->Encode(*pb.add_marker());
	}
}

Ant::Ptr Ant::FromSaved(const fort::myrmidon::pb::AntMetadata & pb) {
	auto res = std::make_shared<Ant>(pb.id());
	for( const auto & i : pb.marker() ) {
		res->d_identifications.push_back(Identification::FromSaved(i,res));
	}
	return res;
}


std::pair<Ant::Identification::List::const_iterator,Ant::Identification::List::const_iterator>
Ant::Identification::SortAndCheckOverlap(Ant::Identification::List::iterator begin,
                                         Ant::Identification::List::iterator end) {

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


const FramePointer::Ptr & Ant::Identification::Start() const {
	return d_start;
}

const FramePointer::Ptr & Ant::Identification::End() const {
	return d_end;
}

Eigen::Vector2d Ant::Identification::TagPosition() const {
	return d_position.block<2,1>(0,0);
}

double Ant::Identification::TagAngle() const {
	return d_position.z();
}

uint32_t Ant::Identification::TagValue() const {
	return d_tagValue;
}

Ant::Ptr Ant::Identification::Target() const {
	auto res = d_target.lock();
	if (!res) {
		throw std::runtime_error("invalid ant reference");
	}
	return res;
}
