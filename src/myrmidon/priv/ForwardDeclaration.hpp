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
class FramePointer;

typedef std::unique_ptr<Experiment>     ExperimentPtr;
typedef std::shared_ptr<Ant>            AntPtr;
typedef std::shared_ptr<Identification> IdentificationPtr;
typedef std::vector<IdentificationPtr>  IdentificationList;
typedef std::shared_ptr<Identifier>     IdentifierPtr;
typedef std::shared_ptr<const FramePointer>   FramePointerPtr;
typedef std::unordered_map<fort::myrmidon::Ant::ID,AntPtr> AntByID;

} // namespace priv

} // namespace myrmidon

} // namespace fort
