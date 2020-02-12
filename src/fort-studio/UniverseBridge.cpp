#include "UniverseBridge.hpp"

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include <QStandardItemModel>
#include <QDebug>

using namespace fort::myrmidon;

Q_DECLARE_METATYPE(priv::TrackingDataDirectoryConstPtr);
Q_DECLARE_METATYPE(priv::Space::Ptr);

UniverseBridge::UniverseBridge( QObject * parent)
	: QObject(parent)
	, d_model(new QStandardItemModel(this) ) {

	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &UniverseBridge::on_model_itemChanged);


}


QAbstractItemModel * UniverseBridge::model() {
	return d_model;
}

void UniverseBridge::on_model_itemChanged(QStandardItem * item) {
	if ( item->data(Qt::UserRole+1).toInt() != SPACE_TYPE || item->column() != 0 ) {
		qDebug() << "Invalid item changed";
		return;
	}

	auto z = item->data(Qt::UserRole+2).value<priv::Space::Ptr>();
	if (item->text() == z->URI().c_str()) {
		return;
	}

	try {
		z->SetName(item->text().toUtf8().data());
	} catch (const std::exception & e) {
		qDebug() << "Could not change name: " << e.what();
		item->setText(z->URI().c_str());
	}
	emit spaceChanged(z);
}


QList<QStandardItem*> UniverseBridge::BuildTDD(const priv::TrackingDataDirectoryConstPtr & tdd) {
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

QList<QStandardItem*> UniverseBridge::BuildSpace(const priv::Space::Ptr & z) {
	auto spaceItem = new QStandardItem(z->URI().c_str());
	spaceItem->setEditable(true);
	spaceItem->setData(SPACE_TYPE,Qt::UserRole+1);
	spaceItem->setData(QVariant::fromValue(z),Qt::UserRole+2);
	RebuildSpaceChildren(spaceItem,z);
	QList<QStandardItem*> res = {spaceItem};
	for(size_t i = 0 ; i < 5; ++i) {
		auto dummyItem = new QStandardItem("");
		dummyItem->setEditable(false);
		dummyItem->setData(SPACE_TYPE,Qt::UserRole+1);
		dummyItem->setData(QVariant::fromValue(z),Qt::UserRole+2);
		res.push_back(dummyItem);
	}
	return res;
}

void UniverseBridge::BuildAll(const std::vector<priv::Space::Ptr> & spaces) {
	d_model->clear();
	d_model->setColumnCount(6);
	auto labels = {tr("URI"),tr("Filepath"),tr("Start Frame"),tr("End Frame"),tr("Start Date"),tr("End Date")};
	d_model->setHorizontalHeaderLabels(labels);
	for (const auto & z : spaces) {
		d_model->invisibleRootItem()->appendRow(BuildSpace(z));
	}
}

const std::vector<fmp::Space::Ptr> UniverseBridge::s_emptySpaces;
const fmp::Space::Universe::TrackingDataDirectoryByURI UniverseBridge::s_emptyTDDs;


const std::vector<priv::Space::Ptr> & UniverseBridge::Spaces() const {
	if ( !d_experiment ) {
		return s_emptySpaces;
	}
	return d_experiment->Spaces();
}

const priv::Space::Universe::TrackingDataDirectoryByURI &
UniverseBridge::TrackingDataDirectories() const {
	if ( !d_experiment ) {
		return s_emptyTDDs;
	}

	return d_experiment->TrackingDataDirectories();
}

Error UniverseBridge::addSpace(const QString & spaceName) {
	if (!d_experiment) {
		return Error("No Experiment");;
	}

	priv::Space::Ptr newSpace;
	try {
		newSpace = d_experiment->CreateSpace(spaceName.toUtf8().data());
	} catch (const std::exception & e) {
		return Error("Could not create space '" + spaceName + "'" + e.what());
	}

	d_model->appendRow(BuildSpace(newSpace));
	emit spaceAdded(newSpace);
	return Error::NONE;
}

Error UniverseBridge::addTrackingDataDirectoryToSpace(const QString & spaceURI,
                                                      const fmp::TrackingDataDirectoryConstPtr & tdd) {
	if (!d_experiment) {
		return Error("No Experiment");
	}
	auto z = d_experiment->LocateSpace(spaceURI.toUtf8().data());
	auto item = LocateSpace(spaceURI);
	if ( !z || item == NULL) {
		return Error("No space '" + spaceURI + "'");
	}


	try {
		z->AddTrackingDataDirectory(tdd);
	} catch (const std::exception & e) {
		return Error("Could not add '" + QString(tdd->URI().c_str())
		             + "' to '" + spaceURI + "':" + e.what());
	}

	RebuildSpaceChildren(item,z);

	emit trackingDataDirectoryAdded(tdd);
	emit spaceChanged(z);
	return Error::NONE;
}

Error UniverseBridge::deleteTrackingDataDirectory(const QString & URI) {
	if ( !d_experiment) {
		return Error("No Experiment");
	}

	auto fi  = d_experiment->LocateTrackingDataDirectory(URI.toUtf8().data());
	if (!fi.first || !fi.second) {
		return Error("Could not found TDD '" + URI + "'");
	}

	auto item = LocateSpace(fi.first->URI().c_str());

	try {
		d_experiment->DeleteTrackingDataDirectory(URI.toUtf8().data());
	} catch ( const std::exception & e) {
		return Error("Could not delete '" + URI + "': " + e.what());
	}


	RebuildSpaceChildren(item,fi.first);

	emit trackingDataDirectoryDeleted(URI);
	emit spaceChanged(fi.first);
	return Error::NONE;
}


QStandardItem * UniverseBridge::LocateSpace(const QString & URI) {
	auto items = d_model->findItems(URI);
	if ( items.size() != 1 ) {
		return NULL;
	}
	return items[0];
}

void UniverseBridge::RebuildSpaceChildren(QStandardItem * item,
                                           const fmp::Space::Ptr & z) {
	item->removeRows(0,item->rowCount());
	for ( const auto & tdd : z->TrackingDataDirectories() ) {
		auto tddItem = BuildTDD(tdd);
		item->appendRow(tddItem);
	}
}

void UniverseBridge::SetExperiment(const fmp::Experiment::Ptr & experiment) {
	d_experiment = experiment;
	d_model->clear();
	if (!d_experiment) {
		return;
	}

	for (const auto & z : d_experiment->Spaces() ) {
		d_model->appendRow(BuildSpace
		                   (z));
	}
}
