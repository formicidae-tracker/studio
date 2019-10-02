#pragma once

#include <memory>
#include <filesystem>

#include "Experiment.pb.h"
#include "../Ant.hpp"
#include "FramePointer.hpp"

#include "Identification.hpp"

namespace fort {

namespace myrmidon {

namespace priv {

class Ant {
public:
	typedef std::shared_ptr<Ant> Ptr;
	class Estimate {
	public:
		typedef std::vector<Estimate> List;
		Estimate(){};
		Estimate(const Eigen::Vector2d & head,
		         const Eigen::Vector2d & tail,
		         const Eigen::Vector3d & tagPosition,
		         const std::filesystem::path & fromFile);

		Eigen::Vector2d Head() const;
		Eigen::Vector2d Tail() const;
		Eigen::Vector3d TagPosition() const;
		Eigen::Vector3d Invert() const;
		std::filesystem::path FromFile() const;

		void Encode(fort::myrmidon::pb::Estimate & pb) const;
		static Estimate FromSaved(const fort::myrmidon::pb::Estimate & pb);

	private:
		Eigen::Matrix<double,7,1> d_data;
		std::filesystem::path d_fromFile;
	};

	Ant(uint32_t ID);
	~Ant();

	Identification::List & Identifications();
	const Identification::List & ConstIdentifications() const;
	void SortAndCheckIdentifications();


	fort::myrmidon::Ant::ID ID() const {
		return d_ID;
	}

	const std::string & FormatID() const {
		return d_IDStr;
	}

	void Encode(fort::myrmidon::pb::AntMetadata & pb) const;
	static Ptr FromSaved(const fort::myrmidon::pb::AntMetadata & pb, const IdentifierPtr & identifier);


private:


	uint32_t    d_ID;
	std::string d_IDStr;
	Identification::List d_identifications;
};

typedef std::unordered_map<fort::myrmidon::Ant::ID,Ant::Ptr> AntByID;

} //namespace priv

} // namespace myrmidon

} // namespace fort
