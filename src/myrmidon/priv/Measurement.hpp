#pragma once

#include "Types.hpp"
#include "LocatableTypes.hpp"

#include <Eigen/Core>


namespace fort {
namespace myrmidon {
namespace priv {

class Measurement : public Identifiable {
public:
	class Type {
	public:
		typedef uint32_t ID;

		Type(ID TID,const std::string & name);

		const std::string & Name() const;

		ID TID() const;
	private:
		ID          d_TID;
		std::string d_name;
	};

	const static Type::ID HEAD_TAIL_MEASUREMENT = 0;

	typedef std::shared_ptr<Measurement>       Ptr;
	typedef std::shared_ptr<const Measurement> ConstPtr;

	Measurement(const fs::path & parentURI,
	            Type::ID TID,
	            const Eigen::Vector2d & startFromTag,
	            const Eigen::Vector2d & endFromTag,
	            double tagSizePx);

	const fs::path & URI() const override;

	fs::path TagCloseUpURI() const;

	void DecomposeURI(fs::path & tddURI,
	                  FrameID & FID,
	                  TagID & TID) const;


	Type::ID Type() const;

	const Eigen::Vector2d & StartFromTag() const;
	const Eigen::Vector2d & EndFromTag() const;

	double TagSizePx() const;

private:
	Eigen::Vector2d d_start,d_end;
	Type::ID        d_TID;
	fs::path        d_URI;
	double          d_tagSizePx;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
