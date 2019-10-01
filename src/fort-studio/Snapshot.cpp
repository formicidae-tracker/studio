#include "Snapshot.hpp"


uint32_t Snapshot::TagValue() {
	return d_value;
}

Eigen::Vector2d Snapshot::TagPosition() const {
	return d_position.block<2,1>(0,0);
}

double Snapshot::TagAngle() const {
	return d_position.z();
}

const Snapshot::Vector2dList & Snapshot::Corners() const {
	return d_corners;
}

std::filesystem::path Snapshot::ImagePath() const {
	return d_basedir / d_relativeImagePath;
}

void Snapshot::Encode(fort::myrmidon::pb::Snapshot & pb) const {
	pb.Clear();
	pb.set_path(std::filesystem::relative(d_relativeImagePath,d_basedir).generic_string());
	pb.set_tagvalue(d_value);
	pb.set_frame(d_frame);
	auto pos = pb.mutable_position();
	pos->set_x(d_position.x());
	pos->set_y(d_position.y());
	pb.set_angle(d_position.z());

	for ( const auto & c : d_corners ) {
		auto cpb = pb.add_corner();
		cpb->set_x(c.x());
		cpb->set_y(c.y());
	}
}

Snapshot::ConstPtr Snapshot::FromSaved(const fort::myrmidon::pb::Snapshot & pb, const std::filesystem::path & basedir) {
	auto res = std::make_shared<Snapshot>();
	res->d_frame = pb.frame();
	res->d_relativeImagePath = pb.path();
	res->d_basedir = basedir;
	res->d_value = pb.tagvalue();
	res->d_position <<
		pb.position().x(),
		pb.position().y(),
		pb.angle();
	if ( pb.corner_size() != 4 ) {
		throw std::runtime_error(pb.DebugString() + " does not have 4 corners");
	}

	for ( const auto & cpb : pb.corner() ) {
		res->d_corners.push_back(Eigen::Vector2d(cpb.x(),cpb.y()));
	}
	return res;
}

double ComputeAngleFromCorner(const apriltag_detection_t *q) {

	Eigen::Vector2d c0(q->p[0][0],q->p[0][1]);
	Eigen::Vector2d c1(q->p[1][0],q->p[1][1]);
	Eigen::Vector2d c2(q->p[2][0],q->p[2][1]);
	Eigen::Vector2d c3(q->p[3][0],q->p[3][1]);

	Eigen::Vector2d delta = (c1 + c2) / 2.0 - (c0 + c3) / 2.0;


	return atan2(delta.y(),delta.x());
}


Snapshot::ConstPtr Snapshot::FromApriltag(const apriltag_detection_t * d,
                                          const std::filesystem::path & imagePath,
                                          const std::filesystem::path & basedir,
                                          uint64_t frame) {
	auto res = std::make_shared<Snapshot>();
	res->d_basedir = basedir;
	res->d_relativeImagePath = std::filesystem::relative(imagePath,basedir);
	res->d_frame = frame;
	res->d_value = d->id;

	res->d_position <<
		d->c[0],
		d->c[1],
		ComputeAngleFromCorner(d);
	for(size_t i = 0; i < 4; ++i) {
		res->d_corners.push_back(Eigen::Vector2d(d->p[i][0],d->p[i][1]));
	}
	return res;
}

std::filesystem::path Snapshot::Path() const {
	std::ostringstream os;
	os << d_frame << "/" << d_value;
	return d_basedir / os.str();
}
