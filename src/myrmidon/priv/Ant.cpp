#include "Ant.hpp"

#include <sstream>
#include <iomanip>


using namespace fort::myrmidon::priv;

std::string Ant::FormatID(fort::myrmidon::Ant::ID ID) {
	std::ostringstream os;
	os << "0x" << std::setw(4) << std::setfill('0') << std::hex
	   << std::uppercase << ID;
	return os.str();
}

Ant::Ant(uint32_t ID)
	: d_ID(ID)
	, d_IDStr(FormatID(ID)) {
}

Ant::~Ant() {
}


Identification::List & Ant::Accessor::Identifications(Ant & a){
	return a.d_identifications;
}

const Identification::List & Ant::Identifications() const {
	return d_identifications;
}

void Ant::SortAndCheckIdentifications() {
	auto firstOverlap = Identification::SortAndCheckOverlap(d_identifications.begin(),
	                                                        d_identifications.end());
	if ( firstOverlap.first != firstOverlap.second) {
		throw OverlappingIdentification(**(firstOverlap.first),**(firstOverlap.second));
	}
}


// Ant::Estimate::Estimate(const Eigen::Vector2d & head,
//                         const Eigen::Vector2d & tail,
//                         const Eigen::Vector3d & tagPosition,
//                         const std::filesystem::path & fromFile)
// 	: d_fromFile(fromFile) {
// 	d_data.block<2,1>(0,0) = head;
// 	d_data.block<2,1>(2,0) = tail;
// 	d_data.block<3,1>(4,0) = tagPosition;
// }

// Eigen::Vector2d Ant::Estimate::Head() const {
// 	return d_data.block<2,1>(0,0);
// }

// Eigen::Vector2d Ant::Estimate::Tail() const {
// 	return d_data.block<2,1>(2,0);
// }

// Eigen::Vector3d Ant::Estimate::TagPosition() const {
// 	return d_data.block<3,1>(4,0);
// }

// Eigen::Vector3d Ant::Estimate::Invert() const {
// 	return Eigen::Vector3d::Zero();
// }

// std::filesystem::path Ant::Estimate::FromFile() const {
// 	return d_fromFile;
// }

// void Ant::Estimate::Encode(fort::myrmidon::pb::Estimate & pb) const {
// 	pb.set_fromfile(d_fromFile.generic_string());
// 	pb.set_xhead(d_data(0,0));
// 	pb.set_yhead(d_data(1,0));
// 	pb.set_xtail(d_data(2,0));
// 	pb.set_ytail(d_data(3,0));
// 	pb.set_xposition(d_data(4,0));
// 	pb.set_yposition(d_data(5,0));
// 	pb.set_thetaposition(d_data(6,0));
// }


// Ant::Estimate Ant::Estimate::FromSaved(const fort::myrmidon::pb::Estimate & pb) {
// 	Estimate res;
// 	res.d_fromFile = std::filesystem::path(pb.fromfile(),std::filesystem::path::generic_format);
// 	res.d_data <<
// 		pb.xhead(),
// 		pb.yhead(),
// 		pb.xtail(),
// 		pb.ytail(),
// 		pb.xposition(),
// 		pb.yposition(),
// 		pb.thetaposition();
// 	return res;
// }


// void Ant::Encode(fort::myrmidon::pb::AntMetadata & pb) const {
// 	pb.Clear();
// 	pb.set_id(d_ID);
// 	for ( const auto & i : d_identifications ) {
// 		i->Encode(*pb.add_marker());
// 	}
// }

// void Ant::Load(const fort::myrmidon::pb::AntMetadata & pb,
//                const IdentifierPtr & identifier) {
// 	d_ID = pb.id();
// }
