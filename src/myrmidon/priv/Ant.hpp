#pragma once

#include <memory>
#include <filesystem>

#include "Experiment.pb.h"
#include "../Ant.hpp"
#include "FramePointer.hpp"


namespace fort {

namespace myrmidon {

namespace priv {

class Ant {
public:
	typedef std::shared_ptr<Ant> Ptr;
	class Estimate {
	public:
		typedef std::vector<Estimate> List;

		Estimate(const Eigen::Vector2d & head,
		         const Eigen::Vector2d & tail,
		         const Eigen::Vector3d & tagPosition,
		         const std::filesystem::path & fromFile);

		Eigen::Vector2d Head() const;
		Eigen::Vector2d Tail() const;
		Eigen::Vector3d TagPosition() const;
		Eigen::Vector3d Invert() const;
		std::filesystem::path FromFile() const;
	private:
		Eigen::Matrix<double,7,1> d_data;
		std::filesystem::path d_fromFile;
	};

	class Identification {
	public:
		typedef std::vector<Identification> List;
		void Update();

		FramePointer::Ptr Start;
		FramePointer::Ptr End;
		Estimate::List    Estimates;
		Eigen::Vector3d   Position;
		int32_t           TagValue;
	};

	class OverlappingIdentification : public std::runtime_error {
	public:
		OverlappingIdentification(const Identification & a,
		                          const Identification & b);
	private:
		static std::string Reason(const Identification & a,
		                          const Identification & b);
	};


	Ant(fort::myrmidon::pb::AntMetadata * ant);
	~Ant();

	Identification::List & Identifications();
	void SortAndCheckIdentifications();

	const fort::myrmidon::pb::AntMetadata * Metadata() const {
		return d_metadata.get();
	}

	fort::myrmidon::Ant::ID ID() const {
		return d_metadata->id();
	}

	const std::string & FormatID() const {
		return d_ID;
	}


	void CheckIdentification();


private:
	std::shared_ptr<fort::myrmidon::pb::AntMetadata> d_metadata;

	std::string d_ID;
	Identification::List d_identifications;
};

} //namespace priv

} // namespace myrmidon

} // namespace fort
