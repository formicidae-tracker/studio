#include "CapsuleCollisionDetecter.hpp"
#include "ui_CapsuleCollisionDetecter.h"

#include <QDebug>

#include <Eigen/Core>
#include <Eigen/Geometry>

typedef Eigen::AlignedBox<double,2> AABB;

CapsuleCollisionDetecter::CapsuleCollisionDetecter(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::CapsuleCollisionDetecter)
	, d_scene(new QGraphicsScene) {
	d_ui->setupUi(this);

	d_ui->graphicsView->setScene(d_scene);

	std::vector<QDoubleSpinBox*> spinBoxes =
		{
		 d_ui->aC1XBox,d_ui->aC1YBox,d_ui->aC1RBox,
		 d_ui->aC2XBox,d_ui->aC2YBox,d_ui->aC2RBox,
		 d_ui->bC1XBox,d_ui->bC1YBox,d_ui->bC1RBox,
		 d_ui->bC2XBox,d_ui->bC2YBox,d_ui->bC2RBox,
		};

	for ( const auto & spinBox : spinBoxes ) {
		spinBox->setSingleStep(0.1);
		spinBox->setMinimum(-7000.0);
		spinBox->setMaximum(7000.0);
		connect(spinBox,
		        static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		        this,
		        &CapsuleCollisionDetecter::onAnyValueChanged);
	}
	onAnyValueChanged();
}

CapsuleCollisionDetecter::~CapsuleCollisionDetecter() {
	delete d_ui;
}

inline QPointF toQ(const Eigen::Vector2d & v) {
	return QPointF(v.x(),v.y());
}

void CapsuleCollisionDetecter::onAnyValueChanged() {
	const static double SCALE = 100.0;

	Eigen::Vector2d aC1(d_ui->aC1XBox->value(),-d_ui->aC1YBox->value());
	Eigen::Vector2d aC2(d_ui->aC2XBox->value(),-d_ui->aC2YBox->value());
	double aR1 = SCALE * d_ui->aC1RBox->value();
	double aR2 = SCALE * d_ui->aC2RBox->value();
	Eigen::Vector2d bC1(d_ui->bC1XBox->value(),-d_ui->bC1YBox->value());
	Eigen::Vector2d bC2(d_ui->bC2XBox->value(),-d_ui->bC2YBox->value());
	double bR1 = SCALE * d_ui->bC1RBox->value();
	double bR2 = SCALE * d_ui->bC2RBox->value();

	aC1 *= SCALE;
	aC2 *= SCALE;
	bC1 *= SCALE;
	bC2 *= SCALE;

	AABB aabb(aC1 - Eigen::Vector2d(aR1,aR1),aC1 + Eigen::Vector2d(aR1,aR1));
	aabb = aabb.extend(AABB(aC2 - Eigen::Vector2d(aR2,aR2),aC2 + Eigen::Vector2d(aR2,aR2)));
	aabb = aabb.extend(AABB(bC1 - Eigen::Vector2d(aR1,aR1),bC1 + Eigen::Vector2d(bR1,aR1)));
	aabb = aabb.extend(AABB(bC2 - Eigen::Vector2d(bR2,bR2),bC2 + Eigen::Vector2d(bR2,bR2)));
	auto size = aabb.max()-aabb.min();

	Eigen::Vector2d aCC(aC2 - aC1);
	Eigen::Vector2d bCC(bC2 - bC1);


	d_scene->clear();

	QPen none;
	none.setWidth(0);
	QPen thick;
	thick.setWidth(size.mean()/100);
	QPen fine;
	fine.setWidth(size.mean()/200);


	auto drawPoint =
		[&,this](const Eigen::Vector2d & p) {
			static double size = 4;
			d_scene->addRect(QRectF(toQ(p-Eigen::Vector2d(size/2,size/2)),QSizeF(size,size)),none,QColor(0,0,0));
		};

	auto drawCapsule =
		[this] ( const Eigen::Vector2d & C1,
		           const Eigen::Vector2d & C2,
		           double R1,
		           double R2,
		           const QPen & pen) {
			Eigen::Vector2d diff = C2-C1;
			double distance = diff.norm();
			double angle = std::asin((R2-R1)/(distance));
			diff /= distance;

			Eigen::Vector2d normal = Eigen::Rotation2D<double>(-angle)*Eigen::Vector2d(-diff.y(),diff.x());

			Eigen::Vector2d r1Pos = C1 - normal * R1;
			Eigen::Vector2d r2Pos = C2 - normal * R2;
			Eigen::Vector2d r2Opposite = C2 + Eigen::Rotation2D<double>(angle) * Eigen::Vector2d(-diff.y(),diff.x()) * R2;
			QPainterPath path;

			double startAngle1 = ( std::atan2(normal.y(),-normal.x()) ) * 180 / M_PI ;
			double startAngle2 = 180.0 + startAngle1 - 2 * angle  * 180 / M_PI;

			path.moveTo(r1Pos.x(),r1Pos.y());
			path.arcTo(QRect(C1.x() - R1,
			                 C1.y() - R1,
			                 2*R1,
			                 2*R1),
			           startAngle1,
			           180 - 2*angle * 180.0 / M_PI);
			path.lineTo(r2Opposite.x(),r2Opposite.y());
			path.arcTo(QRect(C2.x() - R2,
			                 C2.y() - R2,
			                 2 * R2,
			                 2 * R2),
			           startAngle2,
			           180 + 2*angle * 180.0 / M_PI);
			path.closeSubpath();

			d_scene->addPath(path,pen);
		};




	d_scene->addLine(QLineF(toQ(aC1),toQ(aC2)),thick);
	d_scene->addLine(QLineF(toQ(bC1),toQ(bC2)),thick);

	drawPoint(aC1);
	drawPoint(aC2);
	drawPoint(bC1);
	drawPoint(bC2);



#define project(t,proj,point,start,diff) do {	  \
		t = diff.dot(point-start) / diff.dot(diff); \
		t = std::min(std::max(t,0.0),1.0); \
		proj = start + t * diff; \
	}while(0)

	bool collide = false;

	Eigen::Vector2d proj;
	double t,distSquared,radiusSquared;
#define do_projection(point,start,segment,pRadius1,pRadius2,Radius) do { \
		project(t,proj,point,start,segment); \
	  \
		d_scene->addLine(QLineF(toQ(proj),toQ(point)),fine); \
		drawPoint(proj); \
	  \
		distSquared = (proj-point).squaredNorm(); \
		if (distSquared < 1.e-6 ) { \
			qDebug() << #point << " Projection is too small"; \
			collide = true; \
		} \
		radiusSquared = pRadius1 +  t * (pRadius2-pRadius1) + Radius; \
		radiusSquared *= radiusSquared; \
	  \
		if ( distSquared < radiusSquared ) { \
			qDebug() << #point << "projection t:" << t \
			         << "radius:" << std::sqrt(radiusSquared) \
			         << "distance:" << std::sqrt(distSquared); \
			collide = true; \
		} \
	}while(0)

	do_projection(bC1,aC1,aCC,aR1,aR2,bR1);
	do_projection(bC2,aC1,aCC,aR1,aR2,bR2);
	do_projection(aC1,bC1,bCC,bR1,bR2,aR1);
	do_projection(aC2,bC1,bCC,bR1,bR2,aR2);

	QColor drawColor(0,255,255);
	if ( collide == true ) {
		drawColor = QColor(255,0,255);
	}
	QPen capsulePen;
	capsulePen.setWidth(thick.width());
	capsulePen.setColor(drawColor);

	drawCapsule(aC1,aC2,aR1,aR2,capsulePen);
	drawCapsule(bC1,bC2,bR1,bR2,capsulePen);


	QRectF roi(aabb.min().x(),aabb.min().y(),size.x(),size.y());

	d_ui->graphicsView->fitInView(roi,Qt::KeepAspectRatio);



}
