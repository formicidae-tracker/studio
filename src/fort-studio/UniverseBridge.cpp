#include "UniverseBridge.hpp"

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include <QStandardItemModel>
#include <QDebug>


UniverseBridge::UniverseBridge( QObject * parent)
	: QObject(parent)
	, d_model(new QStandardItemModel(this) ) {

	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &UniverseBridge::onItemChanged);


}

QAbstractItemModel * UniverseBridge::model() {
	return d_model;
}

void UniverseBridge::onItemChanged(QStandardItem * item) {
	if ( item->data(Qt::UserRole+1).toInt() != SPACE_TYPE || item->column() != 0 ) {
		qDebug() << "Invalid item changed";
		return;
	}

	auto z = item->data(Qt::UserRole+2).value<fmp::Space::Ptr>();
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


QList<QStandardItem*> UniverseBridge::buildTDD(const fmp::TrackingDataDirectoryConstPtr & tdd) {
	auto uri = new QStandardItem(tdd->URI().c_str());
	auto path = new QStandardItem(tdd->AbsoluteFilePath().c_str());
	auto start = new QStandardItem(QString::number(tdd->StartFrame()));
	auto end = new QStandardItem(QString::number(tdd->EndFrame()));
	auto startDate = new QStandardItem(tdd->StartDate().DebugString().c_str());
	auto endDate = new QStandardItem(tdd->EndDate().DebugString().c_str());

	QList<QStandardItem*> res = {uri,path,start,end,startDate,endDate};
	for(const auto i : res) {
		i->setEditable(false);
		i->setData(TDD_TYPE,Qt::UserRole+1);
		i->setData(QVariant::fromValue(tdd),Qt::UserRole+2);
	}
	return res;
}

QList<QStandardItem*> UniverseBridge::buildSpace(const fmp::Space::Ptr & z) {
	auto spaceItem = new QStandardItem(z->URI().c_str());
	spaceItem->setEditable(true);
	spaceItem->setData(SPACE_TYPE,Qt::UserRole+1);
	spaceItem->setData(QVariant::fromValue(z),Qt::UserRole+2);
	rebuildSpaceChildren(spaceItem,z);
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

void UniverseBridge::buildAll(const std::vector<fmp::Space::Ptr> & spaces) {
	d_model->clear();
	d_model->setColumnCount(6);
	auto labels = {tr("URI"),tr("Filepath"),tr("Start Frame"),tr("End Frame"),tr("Start Date"),tr("End Date")};
	d_model->setHorizontalHeaderLabels(labels);
	for (const auto & z : spaces) {
		d_model->invisibleRootItem()->appendRow(buildSpace(z));
	}
}

const std::vector<fmp::Space::Ptr> UniverseBridge::s_emptySpaces;
const fmp::Space::Universe::TrackingDataDirectoryByURI UniverseBridge::s_emptyTDDs;


const std::vector<fmp::Space::Ptr> & UniverseBridge::spaces() const {
	if ( !d_experiment ) {
		return s_emptySpaces;
	}
	return d_experiment->Spaces();
}

const fmp::Space::Universe::TrackingDataDirectoryByURI &
UniverseBridge::trackingDataDirectories() const {
	if ( !d_experiment ) {
		return s_emptyTDDs;
	}

	return d_experiment->TrackingDataDirectories();
}

void UniverseBridge::addSpace(const QString & spaceName) {
	if (!d_experiment) {
		return;
	}

	fmp::Space::Ptr newSpace;
	try {
		newSpace = d_experiment->CreateSpace(spaceName.toUtf8().data());
	} catch (const std::exception & e) {
		qWarning() << "Could not create space '" << spaceName
		           <<"': " << e.what();
		return;
	}

	d_model->appendRow(buildSpace(newSpace));
	emit spaceAdded(newSpace);
}

void UniverseBridge::addTrackingDataDirectoryToSpace(const QString & spaceURI,
                                                      const fmp::TrackingDataDirectoryConstPtr & tdd) {
	if (!d_experiment) {
		return;
	}
	auto z = d_experiment->LocateSpace(spaceURI.toUtf8().data());
	auto item = locateSpace(spaceURI);
	if ( !z || item == NULL) {
		return;
	}


	try {
		z->AddTrackingDataDirectory(tdd);
	} catch (const std::exception & e) {
		qWarning() << "Could not add '" <<tdd->URI().c_str()
		           << "' to '" << spaceURI
		           << "': " << e.what();
		return;
	}

	rebuildSpaceChildren(item,z);

	emit trackingDataDirectoryAdded(tdd);
	emit spaceChanged(z);
}

void UniverseBridge::deleteTrackingDataDirectory(const QString & URI) {
	if ( !d_experiment) {
		return ;
	}

	auto fi  = d_experiment->LocateTrackingDataDirectory(URI.toUtf8().data());
	if (!fi.first || !fi.second) {
		return;
	}

	auto item = locateSpace(fi.first->URI().c_str());

	try {
		d_experiment->DeleteTrackingDataDirectory(URI.toUtf8().data());
	} catch ( const std::exception & e) {
		qWarning() << "Could not delete '" << URI
		           <<"': " << e.what();
		return;
	}


	rebuildSpaceChildren(item,fi.first);

	emit trackingDataDirectoryDeleted(URI);
	emit spaceChanged(fi.first);
}


QStandardItem * UniverseBridge::locateSpace(const QString & URI) {
	auto items = d_model->findItems(URI);
	if ( items.size() != 1 ) {
		return NULL;
	}
	return items[0];
}

void UniverseBridge::rebuildSpaceChildren(QStandardItem * item,
                                           const fmp::Space::Ptr & z) {
	item->removeRows(0,item->rowCount());
	for ( const auto & tdd : z->TrackingDataDirectories() ) {
		auto tddItem = buildTDD(tdd);
		item->appendRow(tddItem);
	}
}

void UniverseBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	d_experiment = experiment;
	d_model->clear();
	if (!d_experiment) {
		emit activeStateChanged(false);
		return;
	}

	for (const auto & z : d_experiment->Spaces() ) {
		d_model->appendRow(buildSpace
		                   (z));
	}
	emit activeStateChanged(true);
}
