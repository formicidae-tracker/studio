#pragma once

#include <Eigen/Core>

#include <fort-tags/fort-tags.h>


#include "FrameReference.hpp"
#include "Isometry2D.hpp"

#include <mutex>

typedef struct apriltag_detection apriltag_detection_t;

namespace fort {
namespace myrmidon {
namespace priv {


class TagCloseUp : public Identifiable , public FileSystemLocatable {
public:
	typedef std::shared_ptr<TagCloseUp>       Ptr;
	typedef std::shared_ptr<const TagCloseUp> ConstPtr;
	typedef std::vector<ConstPtr>             List;

	typedef std::vector<Eigen::Vector2d,Eigen::aligned_allocator<Eigen::Vector2d> > Vector2dList;


	class Lister {
	public :
		typedef std::shared_ptr<Lister>                  Ptr;
		typedef std::function<FrameReference (FrameID) > FrameReferenceResolver;
		typedef std::function<List()>                    Loader;

		Lister(const fs::path & absoluteBaseDir,
		       tags::Family f,
		       uint8_t threshold,
		       FrameReferenceResolver resolver);

		static std::multimap<FrameID,std::pair<fs::path,std::shared_ptr<TagID>>> ListFiles(const fs::path & absoluteFilePath);

		std::vector<Loader> PrepareLoaders();

	private:
		typedef std::map<fs::path,List> ByLocalFile;
		typedef std::shared_ptr<apriltag_family_t> ATFamilyPtr;
		typedef std::shared_ptr<apriltag_detector_t> ATDetectorPtr;

		static fs::path CacheFilePath(const fs::path & filepath);
		static ATFamilyPtr LoadFamily(tags::Family family);


		void UnsafeSaveCache();
		void LoadCache();

		ATDetectorPtr CreateDetector();

		fs::path               d_absoluteBaseDir;
		tags::Family           d_family;
		uint8_t                d_threshold;
		FrameReferenceResolver d_resolver;
		ByLocalFile            d_cache;
		std::mutex             d_mutex;
		ATFamilyPtr            d_atfamily;
		bool                   d_cacheModified;
		size_t                 d_parsed;
	};

	static double ComputeAngleFromCorners(const Eigen::Vector2d & c0,
	                                      const Eigen::Vector2d & c1,
	                                      const Eigen::Vector2d & c2,
	                                      const Eigen::Vector2d & c3);

	TagCloseUp(const fs::path & absoluteFilePath,
	           const FrameReference & reference,
	           TagID tid,
	           const Eigen::Vector2d & position,
	           double angle,
	           const Vector2dList & corners);

	TagCloseUp(const fs::path & absoluteFilePath,
	           const FrameReference & reference,
	           const apriltag_detection_t * d);


	virtual ~TagCloseUp();


	const FrameReference & Frame() const;


	const fs::path & URI() const override;

	const fs::path & AbsoluteFilePath() const override;

	TagID TagValue() const;
	const Eigen::Vector2d & TagPosition() const;
	double TagAngle() const;
	const Vector2dList & Corners() const;

	Isometry2Dd ImageToTag() const;


private:
	FrameReference  d_reference;
	fs::path        d_URI;
	fs::path        d_absoluteFilePath;
	TagID           d_tagID;
	Eigen::Vector2d d_tagPosition;
	double          d_tagAngle;
	Vector2dList    d_corners;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


} // namespace priv
} // namespace myrmidon
} // namespace fort

// Formats a TagInFrameReference
// @out the std::ostream to format to
// @p the <fort::myrmidon::priv::TagInFrameReference> to format
// @return a reference to <out>
std::ostream& operator<<(std::ostream & out,
                         const fort::myrmidon::priv::TagCloseUp & tcu);
