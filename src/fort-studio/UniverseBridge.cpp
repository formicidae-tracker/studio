#include "UniverseBridge.hpp"

#include <myrmidon/priv/TrackingDataDirectory.hpp>

#include <QStandardItemModel>
#include <QDebug>


UniverseBridge::UniverseBridge( QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this) ) {

	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &UniverseBridge::onItemChanged);


}

bool UniverseBridge::isActive() const {
	return d_experiment.get() != NULL;
}

QAbstractItemModel * UniverseBridge::model() {
	return d_model;
}

void UniverseBridge::onItemChanged(QStandardItem * item) {
	if ( item->data(Qt::UserRole+1).toInt() != SPACE_TYPE || item->column() != 0 ) {
		qDebug() << "[UniverseBridge]: Invalid item was changed!!!";
		return;
	}

	auto s = item->data(Qt::UserRole+2).value<fmp::Space::Ptr>();
	if (item->text() == s->URI().c_str()) {
		qDebug() << "[UniverseBridge]: Ignoring change event as name is the same";
		return;
	}

	try {
		qDebug() << "[UniverseBridge]: Calling fort::myrmidon::Space::SetName('" << item->text() << "')";
		s->SetName(item->text().toUtf8().data());
	} catch (const std::exception & e) {
		qCritical() << "Could not change name: " << e.what();
		item->setText(s->URI().c_str());
		return;
	}
	qInfo() << "Changed Space name to '" << item->text() << "'";

	setModified(true);
	emit spaceChanged(s);
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

QList<QStandardItem*> UniverseBridge::buildSpace(const fmp::Space::Ptr & s) {
	auto spaceItem = new QStandardItem(s->URI().c_str());
	spaceItem->setEditable(true);
	spaceItem->setData(SPACE_TYPE,Qt::UserRole+1);
	spaceItem->setData(QVariant::fromValue(s),Qt::UserRole+2);
	rebuildSpaceChildren(spaceItem,s);
	QList<QStandardItem*> res = {spaceItem};
	for(size_t i = 0 ; i < 5; ++i) {
		auto dummyItem = new QStandardItem("");
		dummyItem->setEditable(false);
		dummyItem->setData(SPACE_TYPE,Qt::UserRole+1);
		dummyItem->setData(QVariant::fromValue(s),Qt::UserRole+2);
		res.push_back(dummyItem);
	}
	return res;
}

void UniverseBridge::buildAll(const std::vector<fmp::Space::Ptr> & spaces) {
	d_model->clear();
	d_model->setColumnCount(6);
	auto labels = {tr("URI"),tr("Filepath"),tr("Start Frame"),tr("End Frame"),tr("Start Date"),tr("End Date")};
	d_model->setHorizontalHeaderLabels(labels);
	for (const auto & s : spaces) {
		d_model->invisibleRootItem()->appendRow(buildSpace(s));
	}
}

const std::vector<fmp::Space::Ptr> UniverseBridge::s_emptySpaces;
const fmp::Space::Universe::TrackingDataDirectoryByURI UniverseBridge::s_emptyTDDs;


void UniverseBridge::addSpace(const QString & spaceName) {
	if (!d_experiment) {
		return;
	}

	fmp::Space::Ptr newSpace;
	try {
		qDebug() << "[UniverseBridge]: Calling fort::myrmidon::priv::Experiment::Create('" << spaceName << "')";
		newSpace = d_experiment->CreateSpace(spaceName.toUtf8().data());
	} catch (const std::exception & e) {
		qCritical() << "Could not create space '" << spaceName
		            <<"': " << e.what();
		return;
	}

	qInfo() << "Created space '" << spaceName << "'";
	d_model->appendRow(buildSpace(newSpace));
	setModified(true);
	emit spaceAdded(newSpace);
}

void UniverseBridge::addTrackingDataDirectoryToSpace(const QString & spaceURI,
                                                     const fmp::TrackingDataDirectoryConstPtr & tdd) {
	if (!d_experiment) {
		return;
	}
	auto s = d_experiment->LocateSpace(spaceURI.toUtf8().data());
	auto item = locateSpace(spaceURI);
	if ( !s || item == NULL) {
		qWarning() << "Could not locate space '" << spaceURI
		           << "' abording addition of TDD;'" << tdd->URI().c_str()
		           << "'";
		return;
	}


	try {
		s->AddTrackingDataDirectory(tdd);
	} catch (const std::exception & e) {
		qCritical() << "Could not add '" <<tdd->URI().c_str()
		            << "' to '" << spaceURI
		            << "': " << e.what();
		return;
	}

	qInfo() << "Added TDD:'" << tdd->URI().c_str()
	        << "' to Space:'" << spaceURI << "'";
	rebuildSpaceChildren(item,s);

	setModified(true);
	emit trackingDataDirectoryAdded(tdd);
	emit spaceChanged(s);
}

void UniverseBridge::deleteSpace(const QString & URI) {
	auto item = locateSpace(URI);
	if ( !d_experiment || item == NULL ) {
		return;
	}
	try {
		qDebug() << "[UniverseBridge]: Calling fort::myrmidon::priv::Experiment::DeleteSpace('"
		         << URI << "')";
		d_experiment->DeleteSpace(URI.toUtf8().constData());
	} catch ( const std::exception & e) {
		qCritical() << "Could not remove space '" << URI << "': " << e.what();
		return;
	}

	qInfo() << "Deleted Space:'" << URI << "'";
	d_model->removeRows(item->row(),1);

	setModified(true);
	emit spaceDeleted(URI);
}

void UniverseBridge::deleteTrackingDataDirectory(const QString & URI) {
	if ( !d_experiment) {
		return ;
	}

	auto fi  = d_experiment->LocateTrackingDataDirectory(URI.toUtf8().data());
	if (!fi.first || !fi.second) {
		qWarning() << "Could not locate TDD:'" << URI << "', abording its deletion";
		return;
	}

	auto item = locateSpace(fi.first->URI().c_str());

	try {
		d_experiment->DeleteTrackingDataDirectory(URI.toUtf8().data());
	} catch ( const std::exception & e) {
		qCritical() << "Could not delete '" << URI
		            <<"': " << e.what();
		return;
	}

	rebuildSpaceChildren(item,fi.first);

	qInfo() << "Removed TDD:'" << URI << "' from Space '"
	        << fi.first->URI().c_str() << "'";

	setModified(true);
	emit trackingDataDirectoryDeleted(URI);
	emit spaceChanged(fi.first);
}


QStandardItem * UniverseBridge::locateSpace(const QString & URI) {
	auto items = d_model->findItems(URI);
	if ( items.size() != 1 ) {
		qDebug() << "[UniverseBridge]: Could not locate Qt Item '" << URI << "'";
		return NULL;
	}
	return items[0];
}

void UniverseBridge::rebuildSpaceChildren(QStandardItem * item,
                                           const fmp::Space::Ptr & s) {
	item->removeRows(0,item->rowCount());
	for ( const auto & tdd : s->TrackingDataDirectories() ) {
		auto tddItem = buildTDD(tdd);
		item->appendRow(tddItem);
	}
}

void UniverseBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	qDebug() << "[UniverseBride]: set new experiment";
	d_experiment = experiment;
	d_model->clear();
	setModified(false);

	if (!d_experiment) {
		emit activated(false);
		return;
	}

	for (const auto & s : d_experiment->Spaces() ) {
		d_model->appendRow(buildSpace(s));
	}

	emit activated(true);
}


bool UniverseBridge::isDeletable(const QModelIndex & index) const {
	auto item = d_model->itemFromIndex(index);
	if ( item == NULL ) {
		return false;
	}

	switch(item->data(Qt::UserRole+1).toInt()) {
	case SPACE_TYPE:
		return item->data(Qt::UserRole+1).value<fmp::Space::Ptr>()->TrackingDataDirectories().empty();
	case TDD_TYPE:
		return true;
	}

	return false;
}



void UniverseBridge::deleteSelection(const QModelIndexList & selection) {
	std::vector<fs::path> spaceURIs;
	std::vector<fs::path> tddURIs;
	for ( const auto & index : selection ) {
		if ( index.isValid() == false ) {
			continue;
		}

		auto item = d_model->itemFromIndex(index);
		if ( item == NULL ) {
			continue;
		}

		switch(item->data(Qt::UserRole+1).toInt()) {
		case SPACE_TYPE:
			spaceURIs.push_back(item->data(Qt::UserRole+2).value<fmp::Space::Ptr>()->URI());
		case TDD_TYPE:
			tddURIs.push_back(item->data(Qt::UserRole+2).value<fmp::TrackingDataDirectory::ConstPtr>()->URI());
		}
	}

	for ( const auto & uri : tddURIs ) {
		deleteTrackingDataDirectory(uri.c_str());
	}
	for ( const auto & uri : spaceURIs ) {
		deleteSpace(uri.c_str());
	}
}
