#pragma once

#include <opencv2/imgproc.hpp>

#include "KDTree.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

class KDTreePrinter {
public :
	static void Print(cv::Mat & result,
	                  const KDTree<int,double,2>::ConstPtr & tree) {
		typedef typename KDTree<int,double,2>::Node::Ptr NodePtr;
		typedef typename KDTree<int,double,2>::AABB AABB;
		auto offset = tree->d_root->Volume.min() - Eigen::Vector2d(20,20);
		auto size = tree->d_root->Volume.max() - offset + Eigen::Vector2d(20,20);
		result = cv::Mat(size.y(),size.x(),CV_8UC3);
		result.setTo(cv::Vec3b(255,255,255));

		auto toCv = [offset](const Eigen::Vector2d & p) {
			            return cv::Point(p.x() - offset.x(),
			                             p.y() - offset.y());
		            };
		auto drawAABB =
			[&result,toCv](const AABB & volume,
			               cv::Vec3b color,
			               int thickness) {
				cv::line(result,
				         toCv(volume.min()),
				         toCv(Eigen::Vector2d(volume.min().x(),volume.max().y())),
				         color,
				         thickness);
				cv::line(result,
				         toCv(Eigen::Vector2d(volume.min().x(),volume.max().y())),
				         toCv(volume.max()),
				         color,
				         thickness);
				cv::line(result,
				         toCv(volume.max()),
				         toCv(Eigen::Vector2d(volume.max().x(),volume.min().y())),
				         color,
				         thickness);
				cv::line(result,
				         toCv(Eigen::Vector2d(volume.max().x(),volume.min().y())),
				         toCv(volume.min()),
				         color,
				         thickness);

			};

		std::vector<NodePtr> toProcess =
			{
			 tree->d_root,
			};
		std::vector<AABB> volumes =
			{
			 AABB(tree->d_root->Volume),
			};
		drawAABB(volumes[0],cv::Vec3b(0,0,0),4);
		for ( size_t i = 0 ; i < toProcess.size(); ++i) {
			auto n = toProcess[i];
			auto volume = volumes[i];
			auto center = (n->ObjectVolume.min() + n->ObjectVolume.max()) / 2;
			size_t dim = n->Depth % 2;
			if ( n->Lower ) {
				auto newVolume = volume;
				newVolume.max()(dim,0) = center(dim,0);
				toProcess.push_back(n->Lower);
				volumes.push_back(newVolume);
			}
			if ( n->Upper ) {
				auto newVolume = volume;
				newVolume.min()(dim,0) = center(dim,0);
				toProcess.push_back(n->Upper);
				volumes.push_back(newVolume);
			}

			drawAABB(n->ObjectVolume,cv::Vec3b(255,255,0),2);
			drawAABB(n->Volume,cv::Vec3b(0,0,0),1);

			if ( dim == 0 ) {
				cv::line(result,
				         toCv(Eigen::Vector2d(center.x(),volume.min().y())),
				         toCv(Eigen::Vector2d(center.x(),volume.max().y())),
				         cv::Vec3b(0,0,255),
				         2);
			} else {
				cv::line(result,
				         toCv(Eigen::Vector2d(volume.min().x(),center.y())),
				         toCv(Eigen::Vector2d(volume.max().x(),center.y())),
				         cv::Vec3b(255,0,0),
				         2);
			}

			cv::putText(result,
			            std::to_string(n->Object),
			            toCv(center + Eigen::Vector2d(4,4)),
			            cv::FONT_HERSHEY_SIMPLEX,
			            1.0,
			            cv::Vec3b(0,0,0));
			cv::circle(result,toCv(center),4,cv::Vec3b(0,0,0),-1);

		}
	}
};

} // namespace priv
} // namespace myrmidon
} // namespace fort
