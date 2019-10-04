#pragma once

#include <memory>
#include <vector>

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
typedef std::shared_ptr<FramePointer>   FramePointerPtr;

} // namespace priv

} // namespace myrmidon

} // namespace fort
