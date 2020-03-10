#include "Zone.hpp"


namespace fort {
namespace myrmidon {
namespace priv {


Zone::Geometry::Geometry(const std::vector<Shape::ConstPtr> & shapes)
	: d_shapes(shapes) {
	d_AABBs.reserve(d_shapes.size());
	for ( const auto & s : shapes) {
		auto b = s->ComputeAABB();
		if ( d_AABBs.size() == 0 ) {
			d_globalAABB = b;
		} else {
			d_globalAABB.extend(b);
		}
		d_AABBs.push_back(b);
	}
}


const std::vector<Shape::ConstPtr> & Zone::Geometry::Shapes() const {
	return d_shapes;
}

const AABB & Zone::Geometry::GlobalAABB() const {
	return d_globalAABB;
}

const std::vector<AABB> & Zone::Geometry::IndividualAABB() const {
	return d_AABBs;
}

Zone::Definition::Definition(const Zone::Ptr & zone,
                             Geometry::ConstPtr geometry,
                             const Time::ConstPtr & start,
                             const Time::ConstPtr & end)
	: d_zone(zone)
	, d_geometry(geometry) {
	d_start = start;
	d_end = end;
}

const Zone::Geometry::ConstPtr & Zone::Definition::GetGeometry() const {
	return d_geometry;
}

void Zone::Definition::SetGeometry(const Geometry::ConstPtr & geometry) {
	d_geometry = geometry;
}

const Time::ConstPtr & Zone::Definition::Start() const {
	return d_start;
}

const Time::ConstPtr & Zone::Definition::End() const {
	return d_end;
}

void Zone::Definition::SetStart(const Time::ConstPtr & start) {
	SetBound(start,d_end);
}

void Zone::Definition::SetEnd(const Time::ConstPtr & end) {
	SetBound(d_start,end);
}

void Zone::Definition::SetBound(const Time::ConstPtr & start, const Time::ConstPtr & end) {
	auto zone = d_zone.lock();
	if ( !zone ) {
		throw DeletedReference<Zone>();
	}
	auto oldStart = start;
	auto oldEnd = end;
	d_start = start;
	d_end = end;
	auto res = SortAndCheckOverlap(zone->d_definitions.begin(),
	                               zone->d_definitions.end());
	if ( res.first != res.second ) {
		d_start = oldStart;
		d_end = oldEnd;
		throw std::runtime_error("Zone definition would overlaps with another");
	}
}



Zone::Ptr Zone::Create(ID ZID,const std::string & name,const std::string & parentURI) {
	Zone::Ptr res(new Zone(ZID,name,parentURI));
	res->d_itself = res;
	return res;
}

Zone::Definition::Ptr Zone::AddDefinition(const Geometry::ConstPtr & geometry,
                                          const Time::ConstPtr & start,
                                          const Time::ConstPtr & end) {
	auto itself = d_itself.lock();
	if ( !itself ) {
		throw DeletedReference<Zone>();
	}
	auto res = std::make_shared<Definition>(itself,geometry,start,end);
	auto oldDefinitions = d_definitions;
	d_definitions.push_back(res);
	auto check = TimeValid::SortAndCheckOverlap(d_definitions.begin(),d_definitions.end());
	if ( check.first != check.second ) {
		d_definitions = oldDefinitions;
		throw std::runtime_error("Zone definition would overlaps with another");
	}
	return res;
}

const Zone::Definition::List & Zone::Definitions() const {
	return d_definitions;
}

const std::string & Zone::Name() const {
	return d_name;
}

const std::string & Zone::URI() const {
	return d_URI;
}

Zone::ID Zone::ZoneID() const {
	return d_ZID;
}

Zone::Geometry::ConstPtr Zone::AtTime(const Time & t) {
	for ( const auto & d : d_definitions ) {
		if ( d->IsValid(t) == true ) {
			return d->GetGeometry();
		}
	}
	return Geometry::ConstPtr();
}

Zone::Zone(ID ZID,const std::string & name, const std::string & parentURI)
	: d_ZID(ZID)
	, d_name(name)
	, d_URI( (fs::path(parentURI) / "zones" / std::to_string(ZID)).generic_string() ) {
}

} // namespace priv
} // namespace myrmidon
} // namespace fort
