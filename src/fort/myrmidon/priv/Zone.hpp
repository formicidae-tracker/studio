#pragma once


#include "LocatableTypes.hpp"
#include <fort/myrmidon/Shapes.hpp>

#include <fort/myrmidon/Zone.hpp>

#include "Types.hpp"

namespace fort {
namespace myrmidon {
namespace priv {
class ZoneDefinition;
} // namespae priv
} // namespae myrmidon
} // namespae fort

std::ostream & operator<<(std::ostream & out,
                          const fort::myrmidon::priv::ZoneDefinition & definition);


#include "TimeValid.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class Zone;
typedef std::shared_ptr<Zone> ZonePtr;

class ZoneGeometry  {
public:
	typedef std::shared_ptr<const ZoneGeometry> ConstPtr;

	ZoneGeometry(const Shape::List & shapes);

	const AABB & GlobalAABB() const;
	const std::vector<AABB> & IndividualAABB() const;

	bool Contains(const Eigen::Vector2d & point ) const;

private:
	std::vector<AABB> d_AABBs;
	AABB              d_globalAABB;
	std::vector<std::unique_ptr<const Shape>>  d_shapes;
};


class ZoneDefinition : public TimeValid {
public:
	typedef std::shared_ptr<ZoneDefinition>       Ptr;
	typedef std::shared_ptr<const ZoneDefinition> ConstPtr;
	typedef std::vector<Ptr>                      List;
	typedef std::vector<ConstPtr>                 ConstList;
	typedef ZoneGeometry                          Geometry;

	ZoneDefinition(const ZonePtr & zone,
	               const Shape::List & shapes,
	               const Time & start,
	               const Time & end);
	virtual ~ZoneDefinition();

	const Shape::List & Shapes() const;

	void SetShapes(const Shape::List & shapes);

	const Time & Start() const;

	const Time & End() const;

	void SetStart(const Time & start);

	void SetEnd(const Time & end);

private:
	void SetBound(const Time & start, const Time & end);

	std::weak_ptr<Zone> d_zone;
	Shape::List         d_shapes;
};


class Zone : public Identifiable {
public:
	typedef std::shared_ptr<Zone>       Ptr;
	typedef std::shared_ptr<const Zone> ConstPtr;

	typedef ZoneGeometry   Geometry;
	typedef ZoneDefinition Definition;

	virtual ~Zone();

	static Ptr Create(ZoneID ZID,const std::string & name,const std::string & parentURI);

	Definition::Ptr AddDefinition(const Shape::List & shapes,
	                              const Time & start,
	                              const Time & end);

	bool NextFreeTimeRegion(Time & start,Time & end) const;

	const Definition::List & Definitions() const;


	void EraseDefinition(size_t index);

	const std::string & Name() const;

	void SetName(const std::string & name);

	const std::string & URI() const;

	ZoneID ID() const;

	const Shape::List & AtTime(const Time & t);

private:
	friend class ZoneDefinition;

	Zone(ZoneID zoneID,const std::string & name, const std::string & parentURI);

	ZoneID              d_zoneID;
	std::weak_ptr<Zone> d_itself;
	std::string         d_name,d_URI;
	Definition::List    d_definitions;
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
