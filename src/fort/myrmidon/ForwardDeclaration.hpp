#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace fort {
namespace myrmidon {

// Forward declaration for Ant::ID
typedef uint32_t AntID;

class CIdentification;
class Identification;
// Forward declaration  for <Identification::List>
typedef std::vector<Identification> IdentificationList;
// Forward declaration  for <Identification::ConstList>
typedef std::vector<CIdentification> IdentificationConstList;

// forward declaration for <Space::ID>
typedef uint32_t SpaceID;
class Space;

// forward declaration for <Zone::ID>
typedef uint32_t ZoneID;


class Capsule;
// Forward declaration for <Capsule::Ptr>
typedef std::shared_ptr<Capsule> CapsulePtr;
// Forward declaration for <Capsule::ConstPtr>
typedef std::shared_ptr<const Capsule> CapsuleConstPtr;

class Circle;
// Forward declaration for <Circle::Ptr>
typedef std::shared_ptr<Circle> CirclePtr;
// Forward declaration for <Circle::ConstPtr>
typedef std::shared_ptr<const Circle> CircleConstPtr;

class Polygon;
// Forward declaration for <Polygon::Ptr>
typedef std::shared_ptr<Polygon> PolygonPtr;
// Forward declaration for <Polygon::ConstPtr>
typedef std::shared_ptr<const Polygon> PolygonConstPtr;








} // namespace myrmidon
} // namespace fort
