#include "Zone.hpp"

#include <iostream>

#include <fort/myrmidon/Shapes.hpp>

namespace fort {
namespace myrmidon {
namespace priv {


ZoneGeometry::ZoneGeometry(const Shape::List & shapes) {
	d_shapes.reserve(shapes.size());
	for ( const auto & s : shapes ) {
		d_shapes.emplace_back(s->Clone());
	}
	d_AABBs.reserve(d_shapes.size());
	for ( const auto & s : d_shapes) {
		auto b = s->ComputeAABB();
		if ( d_AABBs.size() == 0 ) {
			d_globalAABB = b;
		} else {
			d_globalAABB.extend(b);
		}
		d_AABBs.push_back(b);
	}
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
	                       [&point](const std::unique_ptr<const Shape> & s ) {
		                       return s->Contains(point);
	                       });
	return fi != d_shapes.end();

}

ZoneDefinition::ZoneDefinition(const Zone::Ptr & zone,
                               const Shape::List & shapes,
                               const Time & start,
                               const Time & end)
	: d_zone(zone)
	, d_shapes(shapes) {
	d_start = start;
	d_end = end;
}

ZoneDefinition::~ZoneDefinition() {}

const Shape::List & ZoneDefinition::Shapes() const {
	return d_shapes;
}

void ZoneDefinition::SetShapes(const Shape::List & shapes) {
	d_shapes = shapes;
}

const Time & ZoneDefinition::Start() const {
	return d_start;
}

const Time & ZoneDefinition::End() const {
	return d_end;
}

void ZoneDefinition::SetStart(const Time & start) {
	SetBound(start,d_end);
}

void ZoneDefinition::SetEnd(const Time & end) {
	SetBound(d_start,end);
}

void ZoneDefinition::SetBound(const Time & start, const Time & end) {
	if ( end.Before(start) ) {
		std::ostringstream os;
		os << "Invalid time range [" << start << "," << end << "]";
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



Zone::Ptr Zone::Create(ZoneID zoneID,const std::string & name,const std::string & parentURI) {
	Zone::Ptr res(new Zone(zoneID,name,parentURI));
	res->d_itself = res;
	return res;
}

myrmidon::ZoneDefinition::Ptr Zone::PublicAddDefinition(const Shape::List & shapes,
                                                        const Time & start,
                                                        const Time & end) {
	auto itself = d_itself.lock();
	if ( !itself ) {
		throw DeletedReference<Zone>();
	}
	auto def = std::make_shared<Definition>(itself,shapes,start,end);
	auto oldDefinitions = d_definitions;
	d_definitions.push_back(def);
	auto check = TimeValid::SortAndCheckOverlap(d_definitions.begin(),d_definitions.end());
	if ( check.first != check.second ) {
		d_definitions = oldDefinitions;
		throw std::runtime_error("Zone definition would overlaps with another");
	}
	auto res = std::shared_ptr<myrmidon::ZoneDefinition>(new myrmidon::ZoneDefinition(def));
	d_publicDefinitions.push_back(res);
	TimeValid::SortAndCheckOverlap(d_publicDefinitions.begin(),
	                               d_publicDefinitions.end());

	return res;
}

ZoneDefinition::Ptr Zone::AddDefinition(const Shape::List & shapes,
                                        const Time & start,
                                        const Time & end) {
	auto res = PublicAddDefinition(shapes,start,end);
	return res->d_p;
}

const myrmidon::ZoneDefinition::List & Zone::PublicDefinitions() const {
	return d_publicDefinitions;
}


const ZoneDefinition::List & Zone::Definitions() const {
	return d_definitions;
}

const std::string & Zone::Name() const {
	return d_name;
}

const std::string & Zone::URI() const {
	return d_URI;
}

ZoneID Zone::ID() const {
	return d_zoneID;
}

const Shape::List & Zone::AtTime(const Time & t) {
	for ( const auto & d : d_definitions ) {
		if ( d->IsValid(t) == true ) {
			return d->Shapes();
		}
	}
	throw std::out_of_range("No Zone geometry definition at " + t.Format());
}

Zone::~Zone() {}

Zone::Zone(ZoneID zoneID,const std::string & name, const std::string & parentURI)
	: d_zoneID(zoneID)
	, d_name(name)
	, d_URI( (fs::path(parentURI) / "zones" / std::to_string(zoneID)).generic_string() ) {
}


bool Zone::NextFreeTimeRegion(Time & start,Time & end) const {
	if ( d_definitions.empty() ) {
		start = Time::SinceEver();
		end = Time::Forever();
		return true;
	}
	Time lastEnd = Time::SinceEver();
	for ( const auto & def : d_definitions ) {
		if ( lastEnd == def->Start() ) {
			continue;
		}

		auto t = def->Start().Add(-1);
		try {
			end = TimeValid::UpperUnvalidBound(t,d_definitions.begin(),d_definitions.end());
			start = TimeValid::LowerUnvalidBound(t,d_definitions.begin(),d_definitions.end());
			return true;
		} catch ( const std::invalid_argument &) {
		}
	}

	if ( d_definitions.back()->End().IsForever() == true ) {
		return false;
	}

	auto t = d_definitions.back()->End();
	try {
		end = TimeValid::UpperUnvalidBound(t,d_definitions.begin(),d_definitions.end());
		start = TimeValid::LowerUnvalidBound(t,d_definitions.begin(),d_definitions.end());
		return true;
	} catch ( const std::invalid_argument &) {
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
