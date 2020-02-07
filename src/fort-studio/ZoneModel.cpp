#include "ZoneModel.hpp"

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include <QStandardItemModel>
#include <QDebug>

using namespace fort::myrmidon;

Q_DECLARE_METATYPE(priv::TrackingDataDirectoryConstPtr);
Q_DECLARE_METATYPE(priv::Zone::Ptr);

ZoneAndTDDBridge::ZoneAndTDDBridge( QObject * parent)
	: QObject(parent)
	, d_model(new QStandardItemModel(this) ) {

	connect(d_model,
	        SIGNAL(itemChanged(QStandardItem * )),
	        this,
	        SLOT(on_model_itemChanged(QStandardItem *)));


}


QAbstractItemModel * ZoneAndTDDBridge::model() {
	return d_model;
}

void ZoneAndTDDBridge::on_model_itemChanged(QStandardItem * item) {
	if ( item->data(Qt::UserRole+1).toInt() != ZONE_TYPE || item->column() != 0 ) {
		qDebug() << "Invalid item changed";
		return;
	}

	auto z = item->data(Qt::UserRole+2).value<priv::Zone::Ptr>();
	if (item->text() == z->URI().c_str()) {
		return;
	}

	try {
		z->SetName(item->text().toUtf8().data());
	} catch (const std::exception & e) {
		qDebug() << "Could not change name: " << e.what();
		item->setText(z->URI().c_str());
	}
	emit zoneChanged(z);
}


QList<QStandardItem*> ZoneAndTDDBridge::BuildTDD(const priv::TrackingDataDirectoryConstPtr & tdd) {
	auto formatFrameID = [](priv::FrameID FID) -> QString {
		                     return std::to_string(FID).c_str();
	                     };
	auto formatTime = [](const Time & t) -> QString {
		                  std::ostringstream oss;
		                  oss << t;
		                  return oss.str().c_str();
	                  };
	auto uri = new QStandardItem(tdd->URI().c_str());
	auto path = new QStandardItem(tdd->AbsoluteFilePath().c_str());
	auto start = new QStandardItem(formatFrameID(tdd->StartFrame()));
	auto end = new QStandardItem(formatFrameID(tdd->EndFrame()));
	auto startDate = new QStandardItem(formatTime(tdd->StartDate()));
	auto endDate = new QStandardItem(formatTime(tdd->EndDate()));

	QList<QStandardItem*> res = {uri,path,start,end,startDate,endDate};
	for(const auto i : res) {
		i->setEditable(false);
		i->setData(TDD_TYPE,Qt::UserRole+1);
		i->setData(QVariant::fromValue(tdd),Qt::UserRole+2);
	}
	return res;
}

QList<QStandardItem*> ZoneAndTDDBridge::BuildZone(const priv::Zone::Ptr & z) {
	auto zoneItem = new QStandardItem(z->URI().c_str());
	zoneItem->setEditable(true);
	zoneItem->setData(ZONE_TYPE,Qt::UserRole+1);
	zoneItem->setData(QVariant::fromValue(z),Qt::UserRole+2);
	RebuildZoneChildren(zoneItem,z);
	QList<QStandardItem*> res = {zoneItem};
	for(size_t i = 0 ; i < 5; ++i) {
		auto dummyItem = new QStandardItem("");
		dummyItem->setEditable(false);
		dummyItem->setData(ZONE_TYPE,Qt::UserRole+1);
		dummyItem->setData(QVariant::fromValue(z),Qt::UserRole+2);
		res.push_back(dummyItem);
	}
	return res;
}

void ZoneAndTDDBridge::BuildAll(const std::vector<priv::Zone::Ptr> & zones) {
	d_model->clear();
	d_model->setColumnCount(6);
	auto labels = {tr("URI"),tr("Filepath"),tr("Start Frame"),tr("End Frame"),tr("Start Date"),tr("End Date")};
	d_model->setHorizontalHeaderLabels(labels);
	for (const auto & z : zones) {
		d_model->invisibleRootItem()->appendRow(BuildZone(z));
	}
}


const std::vector<priv::Zone::Ptr> ZoneAndTDDBridge::Zones() const {
	return d_experiment->Zones();
}

const priv::Zone::Group::TrackingDataDirectoryByURI &
ZoneAndTDDBridge::TrackingDataDirectories() const {
	return d_experiment->TrackingDataDirectories();
}

Error ZoneAndTDDBridge::addZone(const QString & zoneName) {
	priv::Zone::Ptr newZone;
	try {
		newZone = d_experiment->CreateZone(zoneName.toUtf8().data());
	} catch (const std::exception & e) {
		return Error("Could not create zone '" + zoneName + "'" + e.what());
	}

	d_model->appendRow(BuildZone(newZone));
	emit zoneAdded(newZone);
	return Error::NONE;
}

Error ZoneAndTDDBridge::addTrackingDataDirectoryToZone(const QString & zoneURI,
                                                      const fmp::TrackingDataDirectoryConstPtr & tdd) {
	auto z = d_experiment->LocateZone(zoneURI.toUtf8().data());
	auto item = LocateZone(zoneURI);
	if ( !z || item == NULL) {
		return Error("No zone '" + zoneURI + "'");
	}


	try {
		z->AddTrackingDataDirectory(tdd);
	} catch (const std::exception & e) {
		return Error("Could not add '" + QString(tdd->URI().c_str())
		             + "' to '" + zoneURI + "':" + e.what());
	}

	RebuildZoneChildren(item,z);

	emit trackingDataDirectoryAdded(tdd);
	emit zoneChanged(z);
	return Error::NONE;
}

Error ZoneAndTDDBridge::deleteTrackingDataDirectory(const QString & URI) {

	auto fi  = d_experiment->LocateTrackingDataDirectory(URI.toUtf8().data());
	if (!fi.first || !fi.second) {
		return Error("Could not found TDD '" + URI + "'");
	}

	auto item = LocateZone(fi.first->URI().c_str());

	try {
		d_experiment->DeleteTrackingDataDirectory(URI.toUtf8().data());
	} catch ( const std::exception & e) {
		return Error("Could not delete '" + URI + "': " + e.what());
	}


	RebuildZoneChildren(item,fi.first);

	emit trackingDataDirectoryDeleted(URI);
	emit zoneChanged(fi.first);
	return Error::NONE;
}


QStandardItem * ZoneAndTDDBridge::LocateZone(const QString & URI) {
	auto items = d_model->findItems(URI);
	if ( items.size() != 1 ) {
		return NULL;
	}
	return items[0];
}

void ZoneAndTDDBridge::RebuildZoneChildren(QStandardItem * item,
                                           const fmp::Zone::Ptr & z) {
	item->removeRows(0,item->rowCount());
	for ( const auto & tdd : z->TrackingDataDirectories() ) {
		auto tddItem = BuildTDD(tdd);
		item->appendRow(tddItem);
	}
}

void ZoneAndTDDBridge::SetExperiment(fmp::Experiment * experiment) {
	d_experiment = experiment;
	d_model->clear();
	if (d_experiment == NULL) {
		return;
	}

	for (const auto & z : d_experiment->Zones() ) {
		d_model->appendRow(BuildZone(z));
	}
}
