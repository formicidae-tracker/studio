#include "TaggingWidget.hpp"
#include "ui_TaggingWidget.h"

#include <QPainter>

TaggingWidget::TaggingWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TaggingWidget)
	, d_controller(NULL) {
    d_ui->setupUi(this);

    qRegisterMetaType<QVector<Snapshot::ConstPtr>>("QVector<Snapshot::ConstPtr>");
    qRegisterMetaType<size_t>("size_t");

    using namespace fort::myrmidon::priv;
    d_ui->familySelector->insertItem(0,"36h11",(int)Experiment::TagFamily::Tag36h11);
    d_ui->familySelector->insertItem(1,"36ARTag",(int)Experiment::TagFamily::Tag36ARTag);
    d_ui->familySelector->insertItem(2,"36h10",(int)Experiment::TagFamily::Tag36h10);
    d_ui->familySelector->insertItem(3,"Standard41h12",(int)Experiment::TagFamily::Standard41h12);
    d_ui->familySelector->insertItem(4,"16h5",(int)Experiment::TagFamily::Tag16h5);
    d_ui->familySelector->insertItem(5,"25h9",(int)Experiment::TagFamily::Tag25h9);
    d_ui->familySelector->insertItem(6,"Circle21h7",(int)Experiment::TagFamily::Circle21h7);
    d_ui->familySelector->insertItem(7,"Circle49h12",(int)Experiment::TagFamily::Circle49h12);
    d_ui->familySelector->insertItem(8,"Custom48h12",(int)Experiment::TagFamily::Custom48h12);
    d_ui->familySelector->insertItem(9,"Standard52h13",(int)Experiment::TagFamily::Standard52h13);
    d_ui->familySelector->setCurrentIndex(-1);
    onNewController(NULL);
    d_ui->tagList->sortByColumn(0,Qt::AscendingOrder);
    d_ui->tagList->setSortingEnabled(true);

    d_ui->imageLabel->setBackgroundRole(QPalette::Base);
    d_ui->imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    // d_ui->imageLabel->setScaledContents(true);

}

TaggingWidget::~TaggingWidget() {
	clearIndexers();
    delete d_ui;
}


void TaggingWidget::onNewController(ExperimentController * controller) {
	if ( d_controller ) {
		disconnect(d_controller,SIGNAL(dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)),
		           this,
		           SLOT(onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)));
	}

	// will cancel all pending parsing
	clearIndexers();
	d_ui->tagList->clear();

	d_controller = controller;
	if ( d_controller == NULL ) {
		d_ui->familySelector->setEnabled(false);
		d_ui->thresholdBox->setEnabled(false);
		d_ui->snapshotProgress->setValue(0);
		return;
	}

	connect(d_controller,SIGNAL(dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)),
	        this,
	        SLOT(onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)));


	auto tf = d_controller->experiment().Family();
	int idx = -1;
	for ( size_t i = 0 ; i < d_ui->familySelector->count(); ++i ) {
		if ( d_ui->familySelector->itemData(i).toInt() == (int)tf ) {
			idx = i;
			break;
		}
	}
	d_ui->familySelector->setCurrentIndex(idx);
	d_ui->familySelector->setEnabled(true);
	d_ui->thresholdBox->setValue(d_controller->experiment().Threshold());
	d_ui->thresholdBox->setEnabled(true);


	onDataDirUpdated(d_controller->experiment().TrackingDataDirectories());
}

void TaggingWidget::onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & tdds) {
	if ( d_controller == NULL ) {
		return;
	}


	if ( d_ui->familySelector->currentIndex() == -1 ) {
		clearIndexers();
		return;
	}

	auto tf = (fort::myrmidon::priv::Experiment::TagFamily)(d_ui->familySelector->currentData().toInt());

	for(const auto & [p,tdd] :  tdds ) {
		if (d_indexers.count(p) != 0 ) {
			continue;
		}
		auto indexer = std::make_shared<SnapshotIndexer>(tdd.Path(),
		                                                 d_controller->experiment().Basedir(),
		                                                 tf,
		                                                 d_ui->thresholdBox->value());
		connect(indexer.get(),SIGNAL(resultReady(const QVector<Snapshot::ConstPtr> & , size_t)),
		        this,SLOT(onNewSnapshots(const QVector<Snapshot::ConstPtr> & , size_t)),
		        Qt::QueuedConnection);
		d_indexers[p] = indexer;
		size_t toAdd = indexer->start();
		d_ui->snapshotProgress->setMaximum(d_ui->snapshotProgress->maximum() + toAdd);
	}

	//todo : removes snapshots that are not present in the possibly removed datadirs;

}


void TaggingWidget::clearIndexers() {
	for(auto & [p,indexer] :  d_indexers ) {
		disconnect(indexer.get(),SIGNAL(resultReady(const QVector<Snapshot::ConstPtr> & , size_t)),
		           this,SLOT(onNewSnapshots(const QVector<Snapshot::ConstPtr> & , size_t)));
	}
	d_indexers.clear();
	d_ui->tagList->clear();
	d_tags.clear();
	d_snapshots.clear();
	d_ui->snapshotProgress->setValue(0);
	d_ui->snapshotProgress->setMaximum(0);
}


void TaggingWidget::on_familySelector_activated(int row) {
	if ( d_controller == NULL ) {
		return;
	}
	if ( row < 0 ) {
		return;
	}
	auto tf = (fort::myrmidon::priv::Experiment::TagFamily)(d_ui->familySelector->itemData(row).toInt());
	if ( tf == d_controller->experiment().Family() ) {
		return;
	}
	d_controller->setTagFamily(tf);
	clearIndexers();
	onDataDirUpdated(d_controller->experiment().TrackingDataDirectories());
}


void TaggingWidget::on_thresholdBox_editingFinished() {
	if ( d_controller == NULL ) {
		return;
	}
	if (d_controller->experiment().Threshold() ==  d_ui->thresholdBox->value() ) {
		return;
	}
	d_controller->setThreshold(d_ui->thresholdBox->value());
	clearIndexers();
	onDataDirUpdated(d_controller->experiment().TrackingDataDirectories());
}


void TaggingWidget::onNewSnapshots(const QVector<Snapshot::ConstPtr> & snapshots, size_t done) {
	d_ui->snapshotProgress->setValue(d_ui->snapshotProgress->value()+1);

	for( const auto & s : snapshots) {
		if ( d_tags.count(s->TagValue()) == 0 ) {
			auto tagWidget = new QTreeWidgetItem(d_ui->tagList);
			tagWidget->setData(0,Qt::DisplayRole,(int)s->TagValue());
			d_tags[s->TagValue()] = tagWidget;

		}
		d_snapshots[s->Path().generic_string()] = s;
		auto tagWidget = d_tags[s->TagValue()];
		auto frameWidget = new QTreeWidgetItem(tagWidget);
		frameWidget->setData(0,Qt::DisplayRole,s->Path().parent_path().generic_string().c_str());
		frameWidget->setData(0,Qt::UserRole,s->Path().generic_string().c_str());
	}
}



void TaggingWidget::on_tagList_itemActivated(QTreeWidgetItem *item, int) {
	if ( item->data(0,Qt::UserRole).toString().isEmpty() || d_controller == NULL) {
		return;
	}
	auto path = item->data(0,Qt::UserRole).toString();
	auto fi = d_snapshots.find(path.toUtf8().constData());
	if ( fi == d_snapshots.end() ) {
		return;
	}
	auto s = fi->second;

	auto imagePath = d_controller->experiment().Basedir() / s->ImagePath();
	QImage image(imagePath.c_str());

	if( image.format() != QImage::Format_RGB888 ) {
		image = image.convertToFormat(QImage::Format_RGB888);
	}
	int roiSize = std::min(600,std::min(image.width(),image.height()));
	QRect roi(s->TagPosition().x() - roiSize/2,
	          s->TagPosition().y() - roiSize/2,
	          roiSize,roiSize);

	if (roi.x() < 0 ) {
		roi.setX(0);
	}
	if ( roi.x() + roi.width() > image.width() ) {
		roi.setX(std::max(0,image.width() - roiSize));
		roi.setWidth(std::min(roiSize,image.width()-roi.x()));
	}

	if (roi.y() < 0 ) {
		roi.setY(0);
	}
	if ( roi.y() + roi.height() > image.height() ) {
		roi.setY(std::max(0,image.height() - roiSize));
		roi.setHeight(std::min(roiSize,image.height()-roi.y()));
	}

	image = image.copy(roi);

	QPixmap px;
	px.convertFromImage(image,Qt::ColorOnly);
	QPainter painter;
	painter.begin(&px);
	QPen tagPen(QColor(255,0,0),2);
	if ( s->Corners().size() == 4 ) {
		painter.setPen(tagPen);
		painter.drawLine(s->Corners()[0].x() - roi.x(),
		                 s->Corners()[0].y() - roi.y(),
		                 s->Corners()[1].x() - roi.x(),
		                 s->Corners()[1].y() - roi.y());

		painter.drawLine(s->Corners()[1].x() - roi.x(),
		                 s->Corners()[1].y() - roi.y(),
		                 s->Corners()[2].x() - roi.x(),
		                 s->Corners()[2].y() - roi.y());

		painter.drawLine(s->Corners()[2].x() - roi.x(),
		                 s->Corners()[2].y() - roi.y(),
		                 s->Corners()[3].x() - roi.x(),
		                 s->Corners()[3].y() - roi.y());

		painter.drawLine(s->Corners()[3].x() - roi.x(),
		                 s->Corners()[3].y() - roi.y(),
		                 s->Corners()[0].x() - roi.x(),
		                 s->Corners()[0].y() - roi.y());


	}


	d_ui->imageLabel->setPixmap(px);




}
