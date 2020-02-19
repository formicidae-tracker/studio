#pragma once

#include <memory>

#include <Eigen/Geometry>

namespace cv {
class Mat;
};


namespace fort {
namespace myrmidon {
namespace priv {



template <typename T, typename Scalar, int AmbientDim>
class KDTree {
public:
	typedef std::shared_ptr<const KDTree> ConstPtr;
	typedef Eigen::AlignedBox<Scalar,AmbientDim> AABB;

	struct Element {
		T    Object;
		AABB Volume;
	};

	template <typename Iter>
	static ConstPtr Build(const Iter & begin, const Iter & end, int medianDepth);

	std::pair<size_t,size_t> ElementSeparation() const;

	enum { NeedsToAlign = (sizeof(AABB)%16)==0 };
	struct Node {
		typedef std::shared_ptr<Node> Ptr;
		T      Object;
		size_t Depth;
		AABB   Volume,ObjectVolume;
		Ptr    Lower,Upper;
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(NeedsToAlign)
	};

	static Scalar Bound(const AABB & volume, size_t dim);

	template <typename Iter>
	static Iter MedianOf3(Iter lower, Iter middle, Iter upper, size_t dim);

	template <typename Iter>
	static Iter MedianEstimate(const Iter & begin, const Iter & end, size_t dim, size_t depth=0);


	template <typename Iter>
	static typename Node::Ptr Build(const Iter & begin, const Iter & end, size_t depth, int medianDepth);

	static std::pair<size_t,size_t> Elements(const typename Node::Ptr & node);

	typename Node::Ptr d_root;
};

template <typename T>
static void PrintKDTree(cv::Mat & result,
                        const typename KDTree<T,double,2>::ConstPtr & tree);



} // namespace priv
} // namespace myrmidon
} // namespace fort

#include "KDTree.impl.hpp"
