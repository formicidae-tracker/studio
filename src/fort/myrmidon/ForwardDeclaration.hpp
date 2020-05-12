#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace fort {
namespace myrmidon {


// Forward declaration for <Ant::ID>
typedef uint32_t AntID;

class Ant;
// Forward declaration for <Ant::Ptr>
typedef std::shared_ptr<Ant> AntPtr;
// Forward declaration for <Ant::ConstPtr>
typedef std::shared_ptr<const Ant> AntConstPtr;

class Identification;
// Forward declaration for <Identification::Ptr>
typedef std::shared_ptr<Identification> IdentificationPtr;
// Forward declaration for <Identification::ConstPtr>
typedef std::shared_ptr<const Identification> IdentificationConstPtr;
// Forward declaratio  for <Identification::List>
typedef std::vector<IdentificationPtr> IdentificationList;
// Forward declaratio  for <Identification::ConstList>
typedef std::vector<IdentificationConstPtr> IdentificationConstList;

// forward declaration for <Space::ID>
typedef uint32_t SpaceID;
class Space;
// Forward declaration for <Space::Ptr>
typedef std::shared_ptr<Space> SpacePtr;
// Forward declaration for <Space::ConstPtr>
typedef std::shared_ptr<const Space> SpaceConstPtr;

// forward declaration for <Zone::ID>
typedef uint32_t ZoneID;
class Zone;
// Forward declaration for <Zone::Ptr>
typedef std::shared_ptr<Zone> ZonePtr;
// Forward declaration for <Zone::ConstPtr>
typedef std::shared_ptr<const Zone> ZoneConstPtr;

class Capsule;
// Forward declaration for <Capsule::Ptr>
typedef std::shared_ptr<Capsule> CapsulePtr;
// Forward declaration for <Capsule::ConstPtr>
typedef std::shared_ptr<const Capsule> CapsuleConstPtr;





} // namespace myrmidon
} // namespace fort
