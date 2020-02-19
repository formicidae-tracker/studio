#include "KDTree.hpp"

#include <algorithm>
#include <utility>

namespace fort {
namespace myrmidon {
namespace priv {

template<typename T, typename Scalar, int AmbientDim>
template<typename Iter>
inline typename KDTree<T,Scalar,AmbientDim>::ConstPtr
KDTree<T,Scalar,AmbientDim>::Build(const Iter & begin, const Iter & end, int medianDepth) {
	auto res = std::make_shared<KDTree>();
	res->d_root = Build(begin,end,0,medianDepth);
	return res;
};

template<typename T, typename Scalar, int AmbientDim>
inline Scalar
KDTree<T,Scalar,AmbientDim>::Bound(const AABB & volume, size_t dim) {
	return (volume.min()(dim,0) + volume.max()(dim,0))/2.0;
}


template<typename T, typename Scalar, int AmbientDim>
template <typename Iter>
inline Iter
KDTree<T,Scalar,AmbientDim>::MedianOf3(Iter Lower, Iter Middle, Iter Upper, size_t dim) {
#define swap(a,b) do {	  \
		std::swap(a,b); \
		std::swap(p ## a, p ## b); \
	}while(0)

	double pLower = ((Lower->Volume.min() + Lower->Volume.max()) / 2)(dim,0);
	double pMiddle = ((Middle->Volume.min() + Middle->Volume.max()) / 2)(dim,0);
	double pUpper = ((Upper->Volume.min() + Upper->Volume.max()) / 2)(dim,0);

	if ( pMiddle < pLower ) {
		swap(Lower,Middle);
	}
	if ( pUpper < pLower ) {
		swap(Lower,Upper);
	}
	if ( pMiddle < pUpper ) {
		swap(Middle,Upper);
	}
	return Upper;
}


template<typename T, typename Scalar, int AmbientDim>
template<typename Iter>
inline Iter
KDTree<T,Scalar,AmbientDim>::MedianEstimate(const Iter & begin, const Iter & end, size_t dim, size_t depth) {
	auto size = end-begin;
	if ( size < 3 ) {
		return begin;
	}
	if ( depth > 0 ) {
		size /= 3;
		auto a = MedianEstimate(begin,begin+size,dim,depth-1);
		auto b = MedianEstimate(begin+size,begin+size+size,dim,depth-1);
		auto c = MedianEstimate(begin+size+size,end,dim,depth-1);
		return MedianOf3(a,b,c,dim);
	}
	size /= 2;
	return MedianOf3(begin,begin+size,end-1,dim);
}

template<typename T, typename Scalar, int AmbientDim>
template<typename Iter>
typename KDTree<T,Scalar,AmbientDim>::Node::Ptr
KDTree<T,Scalar,AmbientDim>::Build(const Iter & begin, const Iter & end, size_t depth, int medianDepth) {
	auto size = end-begin;
	if ( size == 0 ) {
		return typename Node::Ptr();
	}
	if ( size == 1) {
		//return a Leaf Node
		auto res = std::make_shared<Node>();
		res->Object = begin->Object;
		res->Depth = depth;
		res->Volume = begin->Volume;
		res->ObjectVolume = begin->Volume;
		return res;
	}
	size_t dim = depth % AmbientDim;

	Iter median;
	if ( medianDepth < 0 ) {
		median = begin + (end-begin) / 2;
		std::nth_element(begin,median,end,
		          [dim](const Element & a, const Element & b) {
			          return Bound(a.Volume,dim) < Bound(b.Volume,dim);
		          });
	} else {
		median = MedianEstimate(begin,end,dim,medianDepth);
	}
	auto res = std::make_shared<Node>();
	res->Object = median->Object;
	res->Depth = depth;
	res->ObjectVolume = median->Volume;
	res->Volume = median->Volume;

	if ( medianDepth >= 0 ) {
		Scalar bound = Bound(median->Volume,dim);
		std::vector<Element> lower,upper;
		lower.reserve(size);
		upper.reserve(size);
		for ( Iter it = begin; it != end; ++it ) {
			if ( it == median ) {
				continue;
			}
			if ( Bound(it->Volume,dim) < bound ) {
				lower.push_back(*it);
			} else {
				upper.push_back(*it);
			}
		}
		res->Lower = Build(lower.begin(),lower.end(),depth+1,medianDepth);
		res->Upper = Build(upper.begin(),upper.end(),depth+1,medianDepth);
	} else {
		res->Lower = Build(begin,median,depth+1,medianDepth);
		res->Upper = Build(median+1,end,depth+1,medianDepth);
	}

	if ( res->Lower ) {
		res->Volume.extend(res->Lower->Volume);
	}
	if ( res->Upper ) {
		res->Volume.extend(res->Upper->Volume);
	}
	return res;
}

template<typename T, typename Scalar, int AmbientDim>
std::pair<size_t,size_t>
KDTree<T,Scalar,AmbientDim>::Elements(const typename Node::Ptr & node) {
	if ( !node ) {
		return {0,0};
	}
	std::pair<size_t,size_t> res;
	if ( node->Lower ) {
		auto l = Elements(node->Lower);
		res.first = 1 + l.first + l.second;
	}

	if ( node->Upper ) {
		auto l = Elements(node->Upper);
		res.second = 1 + l.first + l.second;
	}

	return res;
}

template<typename T, typename Scalar, int AmbientDim>
std::pair<size_t,size_t>
KDTree<T,Scalar,AmbientDim>::ElementSeparation() const {
	return Elements(d_root);
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
