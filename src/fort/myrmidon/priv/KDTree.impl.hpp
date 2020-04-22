#include "KDTree.hpp"

#include <algorithm>
#include <utility>

#include <iostream>

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
#define fmp_kdtree_swap(a,b) do {	  \
		std::swap(a,b); \
		std::swap(p ## a, p ## b); \
	}while(0)

	double pLower = ((Lower->Volume.min() + Lower->Volume.max()) / 2)(dim,0);
	double pMiddle = ((Middle->Volume.min() + Middle->Volume.max()) / 2)(dim,0);
	double pUpper = ((Upper->Volume.min() + Upper->Volume.max()) / 2)(dim,0);

	if ( pMiddle < pLower ) {
		fmp_kdtree_swap(Lower,Middle);
	}
	if ( pUpper < pLower ) {
		fmp_kdtree_swap(Lower,Upper);
	}
	if ( pMiddle < pUpper ) {
		fmp_kdtree_swap(Middle,Upper);
	}
#undef fmp_kdtree_swap
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
		res->NLower = lower.size();
		res->Upper = Build(upper.begin(),upper.end(),depth+1,medianDepth);
		res->NUpper = upper.size();
	} else {
		res->Lower = Build(begin,median,depth+1,medianDepth);
		res->NLower = median-begin;
		res->Upper = Build(median+1,end,depth+1,medianDepth);
		res->NUpper = end - (median+1);
	}

	if ( res->Lower ) {
		res->Volume.extend(res->Lower->Volume);
		res->LowerDepth = std::max(res->LowerDepth,res->UpperDepth) + 1;
	} else {
		res->LowerDepth = 0;
	}
	if ( res->Upper ) {
		res->Volume.extend(res->Upper->Volume);
		res->UpperDepth = std::max(res->LowerDepth,res->UpperDepth) + 1;
	} else {
		res->UpperDepth = 0;
	}
	return res;
}


template<typename T, typename Scalar, int AmbientDim>
std::pair<size_t,size_t>
KDTree<T,Scalar,AmbientDim>::ElementSeparation() const {
	return std::make_pair(d_root->NLower,d_root->NUpper);
}

template<typename T, typename Scalar, int AmbientDim>
template <typename OutputIter>
inline void
KDTree<T,Scalar,AmbientDim>::ComputeCollisionForNode(const typename Node::Ptr & node,
                                                     const typename Node::List & possible,
                                                     ReminderList & reminders,
                                                     OutputIter & output) {
	if (!node) {
		return;
	}

	//we do a depth first
	if ( node->Lower) {
		typename Node::List lower;
		lower.reserve(possible.size() + 1);
		for (const auto & n : possible ) {
			if ( node->Lower->Volume.intersects(n->ObjectVolume) ) {
				lower.push_back(n);
			}
		}

		if ( node->Lower->Volume.intersects(node->ObjectVolume)  ) {
			lower.push_back(node);
		}

		if ( node->Upper ) {
			reminders.push_back(std::make_pair(node->Upper,typename Node::List()));
			reminders.back().second.reserve(node->NLower);
		}
		ComputeCollisionForNode(node->Lower,
		                        lower,
		                        reminders,
		                        output);
	}

	if ( node->Upper ) {
		typename Node::List upper;
		size_t staggedSize = 0;
		if ( node->Lower ) {
			staggedSize = reminders.back().second.size();
		}
		upper.reserve(possible.size() + 1 + staggedSize);
		for ( const auto & n : possible ) {
			if ( node->Upper->Volume.intersects(n->ObjectVolume) ) {
				upper.push_back(n);
			}
		}

		if ( node->Lower ) {
			for ( const auto & n : reminders.back().second ) {
				// these ones intersects as its already tested
				upper.push_back(n);
			}
			reminders.pop_back();
		}

		if  ( node->Upper->Volume.intersects(node->ObjectVolume) ) {
			upper.push_back(node);
		}

		ComputeCollisionForNode(node->Upper,
		                        upper,
		                        reminders,
		                        output);
	}

	// test if the current node collide with any of the reminders
	for ( auto & reminder : reminders ) {
		if ( reminder.first->Volume.intersects(node->ObjectVolume) ) {
			reminder.second.push_back(node);
		}
	}

	// test if I collide with any possible nodes
	for ( const auto & n : possible ) {
		if ( node->Object != n->Object
		     && node->ObjectVolume.intersects(n->ObjectVolume) ) {
			if ( n->Object < node->Object ) {
				output = std::make_pair(n->Object,node->Object);
			} else if ( n->Object > node->Object) {
				output = std::make_pair(node->Object,n->Object);
			}
		}
	}
}

template<typename T, typename Scalar, int AmbientDim>
template <typename OutputIter>
inline void
KDTree<T,Scalar,AmbientDim>::ComputeCollisions(OutputIter & iter) const {
	if ( !d_root ) {
		return;
	}
	ReminderList reminders;
	reminders.reserve(Depth());
	ComputeCollisionForNode(d_root,{},reminders,iter);
}

template<typename T, typename Scalar, int AmbientDim>
inline size_t
KDTree<T,Scalar,AmbientDim>::Depth() const {
	if ( !d_root ) { return 0; };
	return std::max(d_root->UpperDepth,d_root->LowerDepth) + 1;
}

template<typename T, typename Scalar, int AmbientDim>
inline void KDTree<T,Scalar,AmbientDim>::Debug(std::ostream & out) const {
	typedef typename Node::Ptr NodePtr;
	std::function<void (const std::string & , const NodePtr & )> PrintNode =
		[&out,&PrintNode] (const std::string & prefix,const NodePtr & node) {
			out << prefix << "+- " << node->Object << std::endl;
			if ( node->Lower ) {
				PrintNode(prefix + "   ",node->Lower);
			}
			if ( node->Upper ) {
				PrintNode(prefix + "   ",node->Upper);
			}
		};
	if ( !d_root) {
		return;
	}
	PrintNode("",d_root);
}




} // namespace priv
} // namespace myrmidon
} // namespace fort
