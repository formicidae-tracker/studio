#include "Zone.hpp"

#include <iostream>

namespace fort {
namespace myrmidon {
namespace priv {


ZoneGeometry::ZoneGeometry(const std::vector<Shape::ConstPtr> & shapes)
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


const std::vector<Shape::ConstPtr> & ZoneGeometry::Shapes() const {
	return d_shapes;
}

const AABB & ZoneGeometry::GlobalAABB() const {
	return d_globalAABB;
}

const std::vector<AABB> & ZoneGeometry::IndividualAABB() const {
	return d_AABBs;
}

bool ZoneGeometry::Contains(const Eigen::Vector2d & point ) const {
	if ( d_globalAABB.contains(point) == false ) {
		return false;
	}
	auto fi = std::find_if(d_shapes.begin(),
	                       d_shapes.end(),
	                       [&point](const Shape::ConstPtr & s ) {
		                       return s->Contains(point);
	                       });
	return fi != d_shapes.end();

}

ZoneDefinition::ZoneDefinition(const Zone::Ptr & zone,
                               Geometry::ConstPtr geometry,
                               const Time::ConstPtr & start,
                               const Time::ConstPtr & end)
	: d_zone(zone)
	, d_geometry(geometry) {
	d_start = start;
	d_end = end;
}

ZoneDefinition::~ZoneDefinition() {}

const ZoneGeometry::ConstPtr & ZoneDefinition::GetGeometry() const {
	return d_geometry;
}

void ZoneDefinition::SetGeometry(const Geometry::ConstPtr & geometry) {
	d_geometry = geometry;
}

const Time::ConstPtr & ZoneDefinition::Start() const {
	return d_start;
}

const Time::ConstPtr & ZoneDefinition::End() const {
	return d_end;
}

void ZoneDefinition::SetStart(const Time::ConstPtr & start) {
	SetBound(start,d_end);
}

void ZoneDefinition::SetEnd(const Time::ConstPtr & end) {
	SetBound(d_start,end);
}

void ZoneDefinition::SetBound(const Time::ConstPtr & start, const Time::ConstPtr & end) {
	if ( !start == false && !end == false && end->Before(*start) ) {
		std::ostringstream os;
		os << "Invalid time range [" << *start << "," << *end << "]";
		throw std::invalid_argument(os.str());
	}

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

ZoneDefinition::Ptr Zone::AddDefinition(const std::vector<Shape::ConstPtr> & shapes,
                                        const Time::ConstPtr & start,
                                        const Time::ConstPtr & end) {
	auto itself = d_itself.lock();
	if ( !itself ) {
		throw DeletedReference<Zone>();
	}
	auto geometry = std::make_shared<Geometry>(shapes);
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

const ZoneDefinition::List & Zone::Definitions() {
	return d_definitions;
}

const ZoneDefinition::ConstList & Zone::CDefinitions() const {
	return reinterpret_cast<const ZoneDefinition::ConstList&>(d_definitions);
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

ZoneGeometry::ConstPtr Zone::AtTime(const Time & t) {
	for ( const auto & d : d_definitions ) {
		if ( d->IsValid(t) == true ) {
			return d->GetGeometry();
		}
	}
	return Geometry::ConstPtr();
}

Zone::~Zone() {}

Zone::Zone(ID ZID,const std::string & name, const std::string & parentURI)
	: d_ZID(ZID)
	, d_name(name)
	, d_URI( (fs::path(parentURI) / "zones" / std::to_string(ZID)).generic_string() ) {
}

static bool TimePtrEqual(const Time::ConstPtr & a,
                  const Time::ConstPtr & b) {
	if ( !a ) {
		return !b;
	}
	if ( !b ) {
		return false;
	}
	return *a == *b;
}



bool Zone::NextFreeTimeRegion(Time::ConstPtr & start,Time::ConstPtr & end) const {
	if ( d_definitions.empty() ) {
		start.reset();
		end.reset();
		return true;
	}
	Time::ConstPtr lastEnd;
	for ( const auto & def : d_definitions ) {
		if ( TimePtrEqual(lastEnd,def->Start()) ) {
			continue;
		}

		auto t = def->Start()->Add(-1);
		try {
			end = TimeValid::UpperUnvalidBound(t,d_definitions.begin(),d_definitions.end());
			start = TimeValid::LowerUnvalidBound(t,d_definitions.begin(),d_definitions.end());
			return true;
		} catch ( const std::invalid_argument &) {
		}
	}

	if ( !d_definitions.back()->End() == true ) {
		start.reset();
		end.reset();
		return false;
	}
	auto t = *d_definitions.back()->End();
	try {
		end = TimeValid::UpperUnvalidBound(t,d_definitions.begin(),d_definitions.end());
		start = TimeValid::LowerUnvalidBound(t,d_definitions.begin(),d_definitions.end());
		return true;
	} catch ( const std::invalid_argument &) {
		start.reset();
		end.reset();
		return false;
	}
}

void Zone::EraseDefinition(size_t index) {
	if ( index >= d_definitions.size() ) {
		throw std::out_of_range(std::to_string(index) + " is out of range [0," + std::to_string(d_definitions.size()) + "[");
	}
	d_definitions.erase(d_definitions.begin() + index);
}


void Zone::SetName(const std::string & name) {
	d_name = name;
}

} // namespace priv
} // namespace myrmidon
} // namespace fort


std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::Zone::Definition & definition) {
	return out << "Zone::Definition";
}
