#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include <myrmidon/Ant.hpp>


#define FORT_MYRMIDON_FDECLARE_CLASS(ClassName) \
	class ClassName; \
	typedef std::shared_ptr<ClassName> ClassName ## Ptr; \
	typedef std::shared_ptr<const ClassName> ClassName ## ConstPtr; \


namespace fort {

namespace myrmidon {

namespace priv {


class Experiment;
// Forward decalation for an <priv::Experiment::Ptr>
typedef std::unique_ptr<Experiment>     ExperimentPtr;

// Forward decalation for an <priv::Ant>
FORT_MYRMIDON_FDECLARE_CLASS(Ant)
// Forward decalation for an <priv::Identification>
FORT_MYRMIDON_FDECLARE_CLASS(Identification)
// Forward decalation for an <priv::Identifier>
FORT_MYRMIDON_FDECLARE_CLASS(Identifier)

class RawFrame;
// Forward decalation for an <priv::RawFrame::ConstPtr>
typedef std::shared_ptr<const RawFrame>   RawFrameConstPtr;

// Forward declaration for a <priv::Capsule>
FORT_MYRMIDON_FDECLARE_CLASS(Capsule)

// Forward declaration for a <priv::MovieSegment>
FORT_MYRMIDON_FDECLARE_CLASS(MovieSegment)

// Forward declaration for a <priv::TagCloseUp>
FORT_MYRMIDON_FDECLARE_CLASS(TagCloseUp)

//Forward declaration for a <priv::AntPoseEstimate>
FORT_MYRMIDON_FDECLARE_CLASS(AntPoseEstimate)

// A Map of <Ant> identified by their <Ant::ID>
typedef std::unordered_map<fort::myrmidon::Ant::ID,AntPtr> AntByID;

// A List of <Identification>
typedef std::vector<IdentificationPtr>  IdentificationList;



} // namespace priv

} // namespace myrmidon

} // namespace fort


#undef FORT_MYRMIDON_FDECLARE_CLASS
