#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace fort {
namespace myrmidon {


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
