#pragma once

#include <memory>
#include <vector>
#include "DenseMap.hpp"
#include "ContiguousIDContainer.hpp"

#include <fort/myrmidon/Ant.hpp>


#define FORT_MYRMIDON_FDECLARE_CLASS(ClassName) \
	class ClassName; \
	typedef std::shared_ptr<ClassName> ClassName ## Ptr; \
	typedef std::shared_ptr<const ClassName> ClassName ## ConstPtr; \


namespace fort {

namespace myrmidon {

namespace priv {


class Experiment;
// Forward decalation for an <priv::Experiment::Ptr>
FORT_MYRMIDON_FDECLARE_CLASS(Experiment)

// Forward decalation for an <priv::Ant>
FORT_MYRMIDON_FDECLARE_CLASS(Ant)
// Forward decalation for an <priv::Identification>
FORT_MYRMIDON_FDECLARE_CLASS(Identification)

// Forward decalation for an <priv::IdentifierIF>
FORT_MYRMIDON_FDECLARE_CLASS(IdentifierIF)

// Forward decalation for an <priv::Identifier>
FORT_MYRMIDON_FDECLARE_CLASS(Identifier)

class TrackingDataDirectory;
// Forward decalation for an <priv::TrackingDataDirectory::ConstPtr>
typedef std::shared_ptr<const TrackingDataDirectory>   TrackingDataDirectoryConstPtr;

class RawFrame;
// Forward decalation for an <priv::RawFrame::ConstPtr>
typedef std::shared_ptr<const RawFrame>   RawFrameConstPtr;


// Forward declaration for a <priv::MovieSegment>
FORT_MYRMIDON_FDECLARE_CLASS(MovieSegment)

// Forward declaration for a <priv::TagCloseUp>
FORT_MYRMIDON_FDECLARE_CLASS(TagCloseUp)

//Forward declaration for a <priv::AntPoseEstimate>
FORT_MYRMIDON_FDECLARE_CLASS(AntPoseEstimate)

//Forward declaration for a <priv::Measurement>
FORT_MYRMIDON_FDECLARE_CLASS(Measurement)


//Forward declaration for a <priv::MeasurementType>
FORT_MYRMIDON_FDECLARE_CLASS(MeasurementType)

//Forward declaration for a <priv::Space>
FORT_MYRMIDON_FDECLARE_CLASS(Space)

//Forward declaration for a <priv::Capsule>
FORT_MYRMIDON_FDECLARE_CLASS(Capsule)

// Forward declaration for a <priv::Polygon>
FORT_MYRMIDON_FDECLARE_CLASS(Polygon)

// Forward declaration for a <priv::Circle>
FORT_MYRMIDON_FDECLARE_CLASS(Circle)

// Forward declaration for a <priv::Zone>
FORT_MYRMIDON_FDECLARE_CLASS(Zone)

// Forward declaration for a <priv::AntShapeType>
FORT_MYRMIDON_FDECLARE_CLASS(AntShapeType)



// A Map of <Ant> identified by their <Ant::ID>
typedef DenseMap<fort::myrmidon::Ant::ID,AntPtr> AntByID;

// A List of <Identification>
typedef std::vector<IdentificationPtr>  IdentificationList;

typedef uint32_t                   SpaceID;
typedef DenseMap<SpaceID,SpacePtr> SpaceByID;

typedef uint32_t                   ZoneID;
typedef DenseMap<ZoneID,ZonePtr> ZoneByID;

// Forward declaration of <priv::MeasurementType::ID>
typedef uint32_t MeasurementTypeID;
// Maps the <MeasurementType> by their <MeasurementType::ID>
typedef DenseMap<MeasurementTypeID,MeasurementTypePtr> MeasurementTypeByID;

typedef uint32_t AntShapeTypeID;
typedef DenseMap<AntShapeTypeID,AntShapeTypePtr>                    AntShapeTypeByID;

FORT_MYRMIDON_FDECLARE_CLASS(AntShapeTypeContainer)

FORT_MYRMIDON_FDECLARE_CLASS(AntMetadata)

} // namespace priv

} // namespace myrmidon

} // namespace fort


#undef FORT_MYRMIDON_FDECLARE_CLASS
