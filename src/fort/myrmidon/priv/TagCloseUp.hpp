#pragma once

#include <Eigen/Core>

#include <fort/tags/fort-tags.hpp>
#include <fort/tags/options.hpp>


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


	class Lister {
	public :
		~Lister();
		typedef std::shared_ptr<Lister>                    Ptr;
		typedef std::function<FrameReference (FrameID) >   FrameReferenceResolver;
		typedef std::function<List()>                      Loader;
		typedef std::function<void(apriltag_family_t *)>   ATFamilyDestructor;
		typedef std::pair<fs::path,std::shared_ptr<TagID>> FileAndFilter;
		typedef std::multimap<FrameID,FileAndFilter>       Listing;

		static Listing ListFiles(const fs::path & absoluteFilePath);
		static std::pair<apriltag_family_t*,ATFamilyDestructor> LoadFamily(tags::Family family);

		static fs::path CacheFilePath(const fs::path & filepath);

		static Ptr Create(const fs::path & absoluteBaseDir,
		                  const  tags::ApriltagOptions & detectorOptions,
		                  FrameReferenceResolver resolver,
		                  bool forceCache = false);


		std::vector<Loader> PrepareLoaders();

		const tags::ApriltagOptions & DetectorOptions() const;

	private:
		typedef std::map<fs::path,List> ByLocalFile;

		Lister(const fs::path & absoluteBaseDir,
		       const tags::ApriltagOptions & detectorOptions,
		       FrameReferenceResolver resolver,
		       bool forceCache = false);

		List LoadFile(const FileAndFilter & f,
		              FrameID frameID,
		              size_t nbFiles);

		List LoadFileFromCache(const fs::path & file);


		void UnsafeSaveCache();
		void LoadCache();

		apriltag_detector_t * CreateDetector();

		std::weak_ptr<Lister>  d_itself;
		fs::path               d_absoluteBaseDir;
		tags::ApriltagOptions  d_detectorOptions;
		FrameReferenceResolver d_resolver;
		std::mutex             d_mutex;
		ByLocalFile            d_cache;
		bool                   d_saveCacheOnDelete;
	};

	static double ComputeAngleFromCorners(const Eigen::Vector2d & c0,
	                                      const Eigen::Vector2d & c1,
	                                      const Eigen::Vector2d & c2,
	                                      const Eigen::Vector2d & c3);

	static std::string FormatURI(const std::string & tddURI,
	                             FrameID frameID,
	                             TagID tagID);

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


	const std::string & URI() const override;

	const fs::path & AbsoluteFilePath() const override;

	TagID TagValue() const;
	const Eigen::Vector2d & TagPosition() const;
	double TagAngle() const;
	const Vector2dList & Corners() const;

	Isometry2Dd ImageToTag() const;

	double TagSizePx() const;

	double Squareness() const;




private:

	FrameReference  d_reference;
	std::string     d_URI;
	fs::path        d_absoluteFilePath;
	TagID           d_tagID;
	Eigen::Vector2d d_tagPosition;
	double          d_tagAngle;
	Vector2dList    d_corners;
	double          d_tagWidthPx,d_squareness;
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
