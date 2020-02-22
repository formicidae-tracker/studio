#include "Capsule.hpp"


#include <Eigen/Geometry>
#include <QPen>

const qreal Capsule::LINE_WIDTH = 1.5;
const int   Capsule::BORDER_OPACITY = 200;
const int   Capsule::FILL_OPACITY = 40;
const qreal Capsule::MIN_DISTANCE = 10.0;


inline Eigen::Vector2d ToEigen(const QPointF & a) {
	return Eigen::Vector2d(a.x(),a.y());
}

Capsule::Capsule(const QPointF & c1, const QPointF & c2,
                 qreal r1, qreal r2,
                 QColor color,
                 UpdatedCallback onUpdated,
                 QGraphicsItem * parent)
	: QGraphicsItemGroup(parent)
	, d_radius1(r1)
	, d_radius2(r2)
	, d_color(color)
	, d_path (new QGraphicsPathItem(this))
	, d_onUpdated(onUpdated) {

	d_c1 = new Handle([this]() {
		                  updateCenter(d_c1);
	                  },
		[this] {
			updateCenter(d_c1);
			editFinished();
		});

	d_c2 = new Handle([this]() {
		                  updateCenter(d_c2);
	                  },
		[this] {
			updateCenter(d_c2);
			editFinished();
		});

	d_c1->setPos(c1);
	d_c2->setPos(c2);
	d_r1 = new Handle([this]() {
		                  updateRadius(d_r1);
	              },
		[this]() {
			updateRadius(d_r1);
			editFinished();
		});
	d_r2 = new Handle([this]() {
		                  updateRadius(d_r2);
	                  },
		[this]() {
			updateRadius(d_r2);
			editFinished();
		});

	setFlags(QGraphicsItem::ItemIsSelectable);

	rebuild();
}

Capsule::~Capsule() {
}


void Capsule::paint(QPainter * painter,
                    const QStyleOptionGraphicsItem * option,
                    QWidget * widget) {
	QColor line = d_color;
	if ( isSelected() == true ) {
		line = d_color.lighter(150);
	}
	line.setAlpha(BORDER_OPACITY);
	QColor fill = line;
	fill.setAlpha(FILL_OPACITY);
	d_path->setPen(QPen(line,LINE_WIDTH));
	d_path->setBrush(fill);
	QGraphicsItemGroup::paint(painter,option,widget);

}


void Capsule::updateCenter(Handle * center) {
	Handle * other = nullptr;
	if ( center == d_c1 ) {
		other = d_c2;
	} else if ( center == d_c2 ) {
		other = d_c1;
	} else {
		//likely an internal logic error, we won't do anything
		return;
	}
	Eigen::Vector2d CC = ToEigen(center->pos()) - ToEigen(other->pos());
	double distance = CC.norm();
	if ( distance < MIN_DISTANCE ) {
		auto newPos = ToEigen(other->pos()) + MIN_DISTANCE / distance * CC;
		center->setPos(newPos.x(),newPos.y());
		distance = MIN_DISTANCE;
	}
	d_radius1 = std::min(std::max(d_radius1,MIN_DISTANCE/2.0),distance);
	d_radius2 = std::min(std::max(d_radius2,MIN_DISTANCE/2.0),distance);
	rebuild();
}

void Capsule::updateRadius(Handle * radius) {
	double * value = nullptr;
	Handle * center = nullptr;
	if ( radius == d_r1 ) {
		value = &d_radius1;
		center = d_c1;
	} else if ( radius == d_r2 ) {
		value = &d_radius2;
		center = d_c2;
	} else {
		//likely an internal logic error, we ignore it
		return;
	}

	double maxRadius = (ToEigen(d_c1->pos()) - ToEigen(d_c2->pos())).norm();
	double distance = (ToEigen(radius->pos()) - ToEigen(center->pos())).norm();
	*value = std::min(std::max(distance,MIN_DISTANCE/2.0),maxRadius);
	rebuild();
}


void Capsule::editFinished() const {
	d_onUpdated(d_c1->pos(),
	            d_c2->pos(),
	            d_radius1,
	            d_radius2);
}

void Capsule::rebuild() {
	Eigen::Vector2d c1(ToEigen(d_c1->pos()));
	Eigen::Vector2d c2(ToEigen(d_c2->pos()));
	Eigen::Vector2d cc = c2 - c1;
	double distance = cc.norm();

	QPainterPath path;
	if (d_radius1 < 1e-6 || d_radius2 < 1e-6 || distance < 1e-6) {
		d_path->setPath(path);
		return;
	}

	double angle = std::asin((d_radius2-d_radius1)/distance);

	cc /= distance;

	Eigen::Vector2d normal = Eigen::Rotation2D<double>(-angle) * Eigen::Vector2d(-cc.y(),cc.x());

	Eigen::Vector2d r1Pos = c1 - normal * d_radius1;
	Eigen::Vector2d r2Pos = c2 - normal * d_radius2;
	Eigen::Vector2d r2Opposite = c2 + Eigen::Rotation2D<double>(angle) * Eigen::Vector2d(-cc.y(),cc.x()) * d_radius2;

	d_r1->setPos(r1Pos.x(),r1Pos.y());
	d_r2->setPos(r2Pos.x(),r2Pos.y());

	double startAngle1 = ( std::atan2(normal.y(),-normal.x()) ) * 180 / M_PI ;
	double startAngle2 = 180.0 + startAngle1 - 2 * angle  * 180 / M_PI;

	path.moveTo(r1Pos.x(),r1Pos.y());
	path.arcTo(QRect(c1.x() - d_radius1,
	                 c1.y() - d_radius1,
	                 2*d_radius1,
	                 2*d_radius1),
	           startAngle1,
	           180 - 2*angle * 180.0 / M_PI);
	path.lineTo(r2Opposite.x(),r2Opposite.y());
	path.arcTo(QRect(c2.x() - d_radius2,
	                 c2.y() - d_radius2,
	                 2 * d_radius2,
	                 2 * d_radius2),
	           startAngle2,
	           180 + 2*angle * 180.0 / M_PI);
	path.closeSubpath();
	d_path->setPath(path);


}
