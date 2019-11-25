#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "../Ant.hpp"


namespace fort {

namespace myrmidon {

namespace priv {

class Experiment;
class Ant;
class Identification;
class Identifier;
class RawFrame;

// Forward decalation for an <Experiment::Ptr>
typedef std::unique_ptr<Experiment>     ExperimentPtr;
// Forward decalation for an <Ant::Ptr>
typedef std::shared_ptr<Ant>            AntPtr;
// Forward decalation for an <Identification::Ptr>
typedef std::shared_ptr<Identification> IdentificationPtr;
// Forward decalation for an <Identifier::Ptr>
typedef std::shared_ptr<Identifier>     IdentifierPtr;
// Forward decalation for an <RawFrame::ConstPtr>
typedef std::shared_ptr<const RawFrame>   RawFrameConstPtr;


// A Map of <Ant> identified by their <Ant::ID>
typedef std::unordered_map<fort::myrmidon::Ant::ID,AntPtr> AntByID;

// A List of <Identification>
typedef std::vector<IdentificationPtr>  IdentificationList;

// The ID of a frame
typedef uint64_t FrameID;

// The ID for a movie frame
typedef uint64_t MovieFrameID;



} // namespace priv

} // namespace myrmidon

} // namespace fort
