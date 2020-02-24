#pragma once

#include "Shape.hpp"
#include "Types.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

class Polygon : public Shape {
public:
	typedef std::shared_ptr<Polygon>       Ptr;
	typedef std::shared_ptr<const Polygon> ConstPtr;

	Polygon(const Vector2dList & vertices);
	~Polygon();

	size_t Size() const;

	const Eigen::Vector2d & Vertex(size_t i) const;

	void SetVertex(size_t i, const Eigen::Vector2d & v);

	bool Contains(const Eigen::Vector2d & point) const override;

private:

	Vector2dList d_vertices;
};


} // namespace priv
} // namespace myrmidon
} // namespace fort
