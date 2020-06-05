#pragma once

#include "Types.hpp"
#include "LocatableTypes.hpp"

#include <Eigen/Core>


namespace fort {
namespace myrmidon {
namespace priv {

class MeasurementType {
public:
	typedef uint32_t                               ID;
	typedef std::shared_ptr<MeasurementType>       Ptr;
	typedef std::shared_ptr<const MeasurementType> ConstPtr;

	MeasurementType(ID TID,const std::string & name);

	const std::string & Name() const;

	void SetName(const std::string & name);

	ID MTID() const;

private:
	ID          d_TID;
	std::string d_name;
};


class Measurement : public Identifiable {
public:

	const static MeasurementType::ID HEAD_TAIL_TYPE;

	typedef std::shared_ptr<Measurement>       Ptr;
	typedef std::shared_ptr<const Measurement> ConstPtr;

	Measurement(const std::string & parentURI,
	            MeasurementType::ID TID,
	            const Eigen::Vector2d & startFromTag,
	            const Eigen::Vector2d & endFromTag,
	            double tagSizePx);

	virtual ~Measurement();

	const std::string & URI() const override;

	std::string TagCloseUpURI() const;

	static void DecomposeURI(const std::string & URI,
	                         std::string & tddURI,
	                         FrameID & FID,
	                         TagID & TID,
	                         MeasurementType::ID & MTID);


	MeasurementType::ID Type() const;

	const Eigen::Vector2d & StartFromTag() const;
	const Eigen::Vector2d & EndFromTag() const;

	double TagSizePx() const;

private:
	Eigen::Vector2d     d_start,d_end;
	MeasurementType::ID d_TID;
	std::string         d_URI;
	double              d_tagSizePx;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
