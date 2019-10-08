#include "TaggingWidget.hpp"
#include "ui_TaggingWidget.h"

#include <iomanip>

#include <QSettings>

#include <myrmidon/utils/NotYetImplemented.hpp>
#include <myrmidon/utils/ProtobufFileReadWriter.hpp>

#include <myrmidon/priv/Identification.hpp>
#include <myrmidon/priv/Ant.hpp>

#include "Estimate.pb.h"

#include "utils.hpp"

using namespace fort::myrmidon::priv;

const std::filesystem::path TaggingWidget::ESTIMATE_SAVE_PATH = "ants/pose_estimates.fortstudio";
const char * TaggingWidget::GOOD_ICON = "emblem-system";
const char * TaggingWidget::BAD_ICON = "emblem-unreadable";

TaggingWidget::TaggingWidget(QWidget *parent)
	: QWidget(parent)
	, d_ui(new Ui::TaggingWidget)
	, d_controller(NULL) {
    d_ui->setupUi(this);

    qRegisterMetaType<QVector<Snapshot::ConstPtr>>("QVector<Snapshot::ConstPtr>");
    qRegisterMetaType<std::vector<uint32_t>>("std::vector<uint32_t>");
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

    QSettings settings;
    if( settings.contains("taggingWidget/ROI") ) {
	    d_ui->roiBox->setValue(settings.value("taggingWidget/ROI").toInt());
    }

    updateButtonState();
}

TaggingWidget::~TaggingWidget() {
	clearIndexers();
    delete d_ui;
}

void TaggingWidget::onNewController(ExperimentController * controller) {
	d_ui->identificationWidget->onNewController(controller);
	if ( d_controller ) {
		disconnect(d_controller,SIGNAL(dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)),
		           this,
		           SLOT(onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)));

		disconnect(d_controller,
		           SIGNAL(identificationCreated(const fort::myrmidon::priv::IdentificationPtr &)),
		           this,
		           SLOT(onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr &)));

		disconnect(d_controller,
		           SIGNAL(identificationDeleted(const fort::myrmidon::priv::IdentificationPtr &)),
		           this,
		           SLOT(onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr &)));
	}

	// will cancel all pending parsing
	clearIndexers();
	d_ui->tagList->clear();

	d_controller = controller;
	if ( d_controller == NULL ) {
		d_ui->familySelector->setEnabled(false);
		d_ui->thresholdBox->setEnabled(false);
		d_ui->snapshotProgress->setValue(0);
		d_ui->unusedTagDisplay->setText(tr("N.A."));
		d_ui->missingTagDisplay->setText(tr("N.A."));
		return;
	}
	d_ui->snapshotViewer->setBasedir(d_controller->experiment().Basedir());
	connect(d_controller,SIGNAL(dataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)),
	        this,
	        SLOT(onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &)));

	connect(d_controller,
	        SIGNAL(identificationCreated(const fort::myrmidon::priv::IdentificationPtr &)),
	        this,
	        SLOT(onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr &)));

	connect(d_controller,
	        SIGNAL(identificationDeleted(const fort::myrmidon::priv::IdentificationPtr &)),
	        this,
	        SLOT(onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr &)));

	d_estimates.clear();

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

	d_used.clear();
	for ( const auto & [ID,a] : d_controller->experiment().ConstIdentifier().Ants() ) {
		for ( const auto & i : a->Identifications() ) {
			d_used.insert(i->TagValue());
		}
	}
	d_allTrackedTags.clear();

	updateUnusedCount();

	onDataDirUpdated(d_controller->experiment().TrackingDataDirectories());
}

void TaggingWidget::onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & tdds) {
	using namespace fort::myrmidon;
	typedef utils::ProtobufFileReadWriter<pb::EstimateHeader,pb::Estimate> ReadWriter;

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
		auto indexer = std::make_shared<SnapshotIndexer>(tdd,
		                                                 d_controller->experiment().Basedir(),
		                                                 tf,
		                                                 d_ui->thresholdBox->value());
		connect(indexer.get(),SIGNAL(resultReady(const QVector<Snapshot::ConstPtr> & , size_t)),
		        this,SLOT(onNewSnapshots(const QVector<Snapshot::ConstPtr> & , size_t)),
		        Qt::QueuedConnection);
		d_indexers[p] = indexer;


		auto extractor = std::make_shared<TagExtractor>();
		connect(extractor.get(),SIGNAL(resultReady(const std::vector<uint32_t> &)),
		        this,SLOT(onNewTrackedTags(const std::vector<uint32_t> & )),
		        Qt::QueuedConnection);
		d_extractors[p] = extractor;
		extractor->start(tdd,d_controller->experiment().Basedir());


		size_t toAdd = indexer->start();
		d_ui->snapshotProgress->setMaximum(d_ui->snapshotProgress->maximum() + toAdd);

		auto path = d_controller->experiment().Basedir() / p / ESTIMATE_SAVE_PATH;
		if ( std::filesystem::exists(path) == false ) {
			continue;
		}

		try {
			ReadWriter::Read(path,
			                 [](const pb::EstimateHeader & h) {
				                 std::ostringstream os;
				                 os << h.majorversion() << "." << h.minorversion();
				                 std::string version = os.str();
				                 if ( version != "0.1" ) {
					                 throw std::invalid_argument("Uncompatible version '" + version + "' (0.1 expected)");
				                 }
			                 },
			                 [this,&tdd](const pb::Estimate & pb) {
				                 try {
					                 auto e = std::make_shared<AntPoseEstimate>(pb::Point2dToEigen(pb.head()),
					                                                            pb::Point2dToEigen(pb.tail()),
					                                                            tdd.FramePointer(pb.frame()),
					                                                            pb.tag());
					                 d_estimates[e->Path()] = e;
				                 } catch ( const std::exception & e )  {
					                 qCritical() << "Invalid protobuf '"  << pb.ShortDebugString().c_str()
					                             << "': " << e.what()
					                             << ";skipping line and continuing";

				                 }
			                 });
		} catch (const std::exception & e) {
			qWarning() << "Could not load pose estimate from '" << path.c_str() << "': " << e.what();
		}
	}

	//todo : removes snapshots that are not present in the possibly removed datadirs;
}


void TaggingWidget::clearIndexers() {
	for(auto & [p,indexer] :  d_indexers ) {
		disconnect(indexer.get(),SIGNAL(resultReady(const QVector<Snapshot::ConstPtr> & , size_t)),
		           this,SLOT(onNewSnapshots(const QVector<Snapshot::ConstPtr> & , size_t)));
	}
	d_indexers.clear();

	for(auto & [p,extractor] :  d_extractors ) {
		disconnect(extractor.get(),SIGNAL(resultReady(const std::vector<uint32_t> &)),
		           this,SLOT(onNewTrackedTags(const std::vector<uint32_t> & )));
	}
	d_extractors.clear();

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
			if ( d_controller != NULL && d_controller->experiment().ConstIdentifier().UseCount(s->TagValue()) != 0 ) {
				tagWidget->setIcon(0,QIcon::fromTheme(GOOD_ICON));
			} else {
				tagWidget->setIcon(0,QIcon::fromTheme(BAD_ICON));
			}
		}
		d_snapshots[s->Path().generic_string()] = s;
		auto tagWidget = d_tags[s->TagValue()];
		auto frameWidget = new QTreeWidgetItem(tagWidget);
		frameWidget->setData(0,Qt::DisplayRole,s->Path().parent_path().generic_string().c_str());
		frameWidget->setData(0,Qt::UserRole,s->Path().generic_string().c_str());
		if (d_estimates.count(s->Path()) != 0 ) {
			frameWidget->setIcon(0,QIcon::fromTheme(GOOD_ICON));
		} else {
			frameWidget->setIcon(0,QIcon::fromTheme(BAD_ICON));
		}
	}

	updateUnusedCount();
}



void TaggingWidget::on_tagList_currentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *) {
	if ( item->data(0,Qt::UserRole).toString().isEmpty() || d_controller == NULL) {
		updateButtonState();
		return;
	}
	auto pathQt = item->data(0,Qt::UserRole).toString();
	std::filesystem::path path(pathQt.toUtf8().constData(),std::filesystem::path::generic_format);
	auto fi = d_snapshots.find(path);
	if ( fi == d_snapshots.end() ) {
		updateButtonState();
		return;
	}
	auto s = fi->second;
	d_ui->snapshotViewer->displaySnapshot(s);
	auto efi = d_estimates.find(path);
	if ( efi != d_estimates.end() ) {
		d_ui->snapshotViewer->setAntPoseEstimate(efi->second);
	}
	auto ident = d_controller->experiment().ConstIdentifier().Identify(s->TagValue(),*s->Frame());
	d_ui->snapshotViewer->displayIdentification(ident);

	updateButtonState();
}


void TaggingWidget::on_roiBox_valueChanged(int value) {
	d_ui->snapshotViewer->setRoiSize(value);
	QSettings settings;
	settings.setValue("taggingWidget/ROI",value);
}


Error TaggingWidget::save() {
	using namespace fort::myrmidon;
	typedef utils::ProtobufFileReadWriter<pb::EstimateHeader,pb::Estimate> ReadWriter;

	std::map<std::filesystem::path,std::vector<AntPoseEstimate::Ptr> > sortedEstimate;
	for(const auto & [p,e] : d_estimates ) {
		sortedEstimate[e->Base()].push_back(e);
	}

	pb::EstimateHeader h;
	h.set_majorversion(0);
	h.set_minorversion(1);
	for ( const auto & [base,estimates] : sortedEstimate ) {


		std::vector<std::function<void (pb::Estimate & )> > lines;
		for (const auto & e : estimates ) {
			lines.push_back([&e](pb::Estimate & pb) {
				                pb.set_frame(e->Frame()->Frame);
				                pb.set_tag(e->TagValue());
				                pb::EigenToPoint2d(pb.mutable_head(),e->Head());
				                pb::EigenToPoint2d(pb.mutable_tail(),e->Tail());
			                });
		}

		auto path = d_controller->experiment().Basedir() / base / ESTIMATE_SAVE_PATH;

		try {
			ReadWriter::Write(path,h,lines);
		} catch (const std::exception & err) {
			return Error("could not save '" + path.string() + "': " + err.what());
		}
	}

	return Error::NONE;
}


void TaggingWidget::on_snapshotViewer_antPoseEstimateUpdated(const AntPoseEstimate::Ptr & e) {
	using namespace fort::myrmidon::priv;
	d_estimates[e->Path()] = e;
	d_controller->setModified(true);

	d_ui->tagList->currentItem()->setIcon(0,QIcon::fromTheme(GOOD_ICON));

	updateIdentificationForCurrentFrame();
	updateButtonState();
}

void TaggingWidget::updateIdentificationForCurrentFrame() {
	auto s = d_ui->snapshotViewer->displayedSnapshot();
	if (!s) {
		return;
	}
	auto ident = updateIdentificationForFrame(s->TagValue(),*(s->Frame()));
	d_ui->snapshotViewer->displayIdentification(ident);
}


Identification::Ptr TaggingWidget::updateIdentificationForFrame(uint32_t tagValue,
                                                                const FramePointer & f) {

	Identification::Ptr ident = d_controller->experiment().ConstIdentifier().Identify(tagValue,f);

	if (!ident) {
		return ident;
	}

	std::vector<std::pair<AntPoseEstimate::Ptr,Snapshot::ConstPtr> > matched;
	for(const auto & [p,ee] : d_estimates ) {
		if (ee->TagValue() != tagValue || !ident->TargetsFrame(*(ee->Frame())) ) {
			continue;
		}
		auto fi = d_snapshots.find(p);
		if ( fi == d_snapshots.end() ) {
			continue;
		}
		matched.push_back(std::make_pair(ee,fi->second));
	}

	if ( matched.size() == 0 ) {
		ident->SetTagPosition(Eigen::Vector2d::Zero(),0);
		return ident;
	}

	Eigen::Vector2d pos(0,0);
	double angle(0);
	for ( const auto & m : matched ) {
		Isometry2Dd tagToAntTransform;


		Identification::ComputeTagToAntTransform(tagToAntTransform,
		                                         m.second->TagPosition(),m.second->TagAngle(),
		                                         m.first->Head(),m.first->Tail());
		pos += tagToAntTransform.translation();
		angle += tagToAntTransform.angle();
	}
	pos /= matched.size();
	angle /= matched.size();
	ident->SetTagPosition(pos,angle);
	d_controller->setModified(true);
	return ident;
}


void TaggingWidget::on_addIdentButton_clicked() {

}

void TaggingWidget::on_newAntButton_clicked() {
	auto e = d_ui->snapshotViewer->antPoseEstimate();
	if (!e) {
		qCritical() << "A pose estimation is needed";
		return;
	}
	auto a = d_controller->createAnt();
	FramePointer::Ptr start;
	FramePointer::Ptr end;
	if ( d_controller->experiment().FreeRangeContaining(start,end,e->TagValue(),*(e->Frame())) == false ) {
		qCritical() << e->Frame()->FullPath().generic_string().c_str() << " already identifies an ant";
		return;
	}

	Error err = d_controller->addIdentification(a->ID(),e->TagValue(),start,end);
	if ( err.OK() == false ) {
		qCritical() << "Could not create identification: " << err.what();
		return;
	}
	updateIdentificationForCurrentFrame();
	updateButtonState();
}


void TaggingWidget::on_deletePoseButton_clicked() {
	auto e = d_ui->snapshotViewer->antPoseEstimate();
	if (!e) {
		qCritical() << "No pose estimate to delete !";
		return;
	}
	auto fi = d_estimates.find(e->Path());
	if (fi == d_estimates.end() ) {
		qCritical() << "Inconsistent state: snaphot pose estimate is not registered.";
		return;
	}
	d_ui->tagList->currentItem()->setIcon(0,QIcon::fromTheme(BAD_ICON));


	d_estimates.erase(fi);
	d_ui->snapshotViewer->setAntPoseEstimate(AntPoseEstimate::Ptr());
	d_controller->setModified(true);
	updateButtonState();
	updateIdentificationForCurrentFrame();

}

void TaggingWidget::onAntSelected(fort::myrmidon::Ant::ID ID) {
	d_selectedAnt = ID;
	updateButtonState();
	//TODO: display Identification in identication widget;
}

void TaggingWidget::updateButtonState() {

	if ( d_controller == NULL ||
	     !d_ui->snapshotViewer->displayedSnapshot() ||
	     !d_ui->snapshotViewer->antPoseEstimate()) {
		d_ui->addIdentButton->setEnabled(false);
		d_ui->newAntButton->setEnabled(false);
		d_ui->deletePoseButton->setEnabled(false);
		return;
	}
	const auto & identifier = d_controller->experiment().ConstIdentifier();

	d_ui->deletePoseButton->setEnabled(true);

	auto e = d_ui->snapshotViewer->antPoseEstimate();
	//we only allow modification
	bool enabled = !identifier.Identify(e->TagValue(),*(e->Frame()));

	if ( enabled == false ) {
		d_ui->addIdentButton->setEnabled(false);
		d_ui->newAntButton->setEnabled(false);
		return;
	}

	d_ui->newAntButton->setEnabled(true);

	//we only allow to add identification if the selected ant is free to accept it
	auto aFi =  identifier.Ants().find(d_selectedAnt);
	if ( aFi == identifier.Ants().end() ) {
		d_ui->addIdentButton->setEnabled(false);
		return;
	}
	auto a = aFi->second;
	for(const auto & ident : a->Identifications() ) {
		if (ident->TargetsFrame(*(e->Frame())) ) {
			d_ui->addIdentButton->setEnabled(false);
			return;
		}
	}


	d_ui->addIdentButton->setEnabled(true);

}


void TaggingWidget::onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr & i ) {
	d_used.insert(i->TagValue());
	updateUnusedCount();
	auto fi = d_tags.find(i->TagValue());
	if ( fi != d_tags.end() ) {
		fi->second->setIcon(0,QIcon::fromTheme(GOOD_ICON));
	}

}

void TaggingWidget::onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr & i ) {
	if (!d_controller) {
		return;
	}
	// the tag is not used anymore
	if (d_controller->experiment().ConstIdentifier().UseCount(i->TagValue()) == 0 ) {
		d_used.erase(i->TagValue());
		auto fi = d_tags.find(i->TagValue());
		if (fi != d_tags.end() ) {
			fi->second->setIcon(0,QIcon::fromTheme(BAD_ICON));
		}

		updateUnusedCount();
	}

	auto s = d_ui->snapshotViewer->displayedSnapshot();
	if ( !s || i->TagValue() != s->TagValue() ||
	     !i->TargetsFrame(*s->Frame()) ) {
		return;
	}
	d_ui->snapshotViewer->displayIdentification(Identification::Ptr());

}

void TaggingWidget::updateUnusedCount() {
	double percent;
	int nb;
	if ( d_used.size() >= d_tags.size() || d_tags.size() == 0) {
		percent = 0.0;
		nb = 0;
	} else {
		nb = d_tags.size() - d_used.size();
		percent = 100.0 * ((double)nb / (double)d_tags.size());
	}
	std::ostringstream os;
	os << nb << "(" << std::fixed << std::setprecision(1) << percent << "%)";
	d_ui->unusedTagDisplay->setText(os.str().c_str());

	if ( d_tags.size() >= d_allTrackedTags.size() || d_allTrackedTags.size() == 0 ) {
		nb = 0;
		percent = 0.0;
	} else {
		nb = d_allTrackedTags.size() - d_tags.size();
		percent = 100.0 * ((double)nb / (double)d_allTrackedTags.size());
	}
	std::ostringstream os2;
	os2 << nb << "(" << std::fixed << std::setprecision(1) << percent << "%)";
	d_ui->missingTagDisplay->setText(os2.str().c_str());

}



void TaggingWidget::onNewTrackedTags(const std::vector<uint32_t> & tags) {
	std::ostringstream os;
	for(auto t : tags){
		os << " " << t;
		d_allTrackedTags.insert(t);
	}
	qDebug() << "tags : " << os.str().c_str();
	updateUnusedCount();
}
