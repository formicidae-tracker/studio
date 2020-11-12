#include "UniverseBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>

#include <fort/myrmidon/priv/TrackingDataDirectory.hpp>

#include <fort/studio/Format.hpp>



UniverseBridge::UniverseBridge( QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this)) {
	qRegisterMetaType<fmp::TrackingDataDirectory::Ptr>();
	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &UniverseBridge::onItemChanged);

}

UniverseBridge::~UniverseBridge(){}

bool UniverseBridge::isActive() const {
	return d_experiment.get() != NULL;
}

QAbstractItemModel * UniverseBridge::model() {
	return d_model;
}

std::map<quint32,QString> UniverseBridge::spaceNamesByID() const {
	std::map<quint32,QString> res;
	if ( !d_experiment == true ) {
		return res;
	}
	for ( const auto [spaceID,space] : d_experiment->Spaces() ) {
		res[spaceID] = space->Name().c_str();
	}
	return res;
}


QString UniverseBridge::basepath() const {
	if ( !d_experiment ) {
		return "";
	}
	return d_experiment->AbsoluteFilePath().parent_path().c_str();
}

void UniverseBridge::onItemChanged(QStandardItem * item) {
	if ( item->data(Qt::UserRole+1).toInt() != SPACE_TYPE || item->column() != 0 ) {
		qDebug() << "[UniverseBridge]: Invalid item was changed!!!";
		return;
	}

	auto s = item->data(Qt::UserRole+2).value<fmp::Space::Ptr>();
	if (item->text() == s->Name().c_str()) {
		qDebug() << "[UniverseBridge]: Ignoring change event as name is the same";
		return;
	}

	try {
		qDebug() << "[UniverseBridge]: Calling fort::myrmidon::Space::SetName('" << item->text() << "')";
		s->SetName(item->text().toUtf8().data());
	} catch (const std::exception & e) {
		qCritical() << "Could not change name: " << e.what();
		item->setText(s->Name().c_str());
		return;
	}
	qInfo() << "Changed Space name to '" << item->text() << "'";

	setModified(true);
	emit spaceChanged(s);
}


QList<QStandardItem*> UniverseBridge::buildTDD(const fmp::TrackingDataDirectory::Ptr & tdd) {
	auto uri = new QStandardItem(tdd->URI().c_str());
	auto path = new QStandardItem(tdd->AbsoluteFilePath().c_str());
	auto start = new QStandardItem(QString::number(tdd->StartFrame()));
	auto end = new QStandardItem(QString::number(tdd->EndFrame()));
	auto startDate = new QStandardItem(ToQString(tdd->StartDate()));
	auto endDate = new QStandardItem(ToQString(tdd->EndDate()));

	QList<QStandardItem*> res = {uri,path,start,end,startDate,endDate};
	for(const auto i : res) {
		i->setEditable(false);
		i->setData(TDD_TYPE,Qt::UserRole+1);
		i->setData(QVariant::fromValue(tdd),Qt::UserRole+2);
	}
	return res;
}

QList<QStandardItem*> UniverseBridge::buildSpace(const fmp::Space::Ptr & s) {
	auto spaceItem = new QStandardItem(s->Name().c_str());
	spaceItem->setEditable(true);
	spaceItem->setData(SPACE_TYPE,Qt::UserRole+1);
	spaceItem->setData(QVariant::fromValue(s),Qt::UserRole+2);
	rebuildSpaceChildren(spaceItem,s);
	QList<QStandardItem*> res = {spaceItem};
	for(size_t i = 0 ; i < 4; ++i) {
		auto dummyItem = new QStandardItem("");
		if ( i == 0 ) {
			dummyItem->setText(QString::number(s->SpaceID()));
		}
		dummyItem->setEditable(false);
		dummyItem->setData(SPACE_TYPE,Qt::UserRole+1);
		dummyItem->setData(QVariant::fromValue(s),Qt::UserRole+2);
		res.push_back(dummyItem);
	}
	return res;
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
		newSpace = d_experiment->CreateSpace(ToStdString(spaceName));
	} catch (const std::exception & e) {
		qCritical() << "Could not create space '" << spaceName
		            <<"': " << e.what();
		return;
	}

	qInfo() << "Created space '" << spaceName << "' with ID" << newSpace->SpaceID();
	if ( newSpace->SpaceID() == d_experiment->Spaces().size() ) {
		d_model->appendRow(buildSpace(newSpace));
	} else {
		d_model->removeRows(0,d_model->rowCount());
		rebuildAll(d_experiment->Spaces());
	}

	setModified(true);
	emit spaceAdded(newSpace);
}

void UniverseBridge::addTrackingDataDirectoryToSpace(const QString & spaceName,
                                                     const fmp::TrackingDataDirectoryPtr & tdd) {
	if (!d_experiment) {
		return;
	}

	auto s = d_experiment->LocateSpace(ToStdString(spaceName));
	auto item = locateSpace(spaceName);
	if ( !s || item == NULL) {
		qWarning() << "Could not locate space '" << spaceName
		           << "' abording addition of TDD;'" << ToQString(tdd->URI())
		           << "'";
		return;
	}


	try {
		d_experiment->AddTrackingDataDirectory(s,tdd);
	} catch (const std::exception & e) {
		qCritical() << "Could not add '" << ToQString(tdd->URI())
		            << "' to '" << spaceName
		            << "': " << e.what();
		return;
	}

	qInfo() << "Added TDD:'" << ToQString(tdd->URI())
	        << "' to Space:'" << spaceName << "'";
	rebuildSpaceChildren(item,s);

	setModified(true);
	emit trackingDataDirectoryAdded(tdd);
	emit spaceChanged(s);
}

void UniverseBridge::deleteSpace(const QString & spaceName) {
	auto item = locateSpace(spaceName);
	if ( !d_experiment || item == NULL ) {
		return;
	}
	auto s = d_experiment->LocateSpace(ToStdString(spaceName));
	if ( !s) {
		qDebug() << "Could not locate space" << spaceName;
	}
	try {
		qDebug() << "[UniverseBridge]: Calling fort::myrmidon::priv::Experiment::DeleteSpace("
		         << s->SpaceID() << ")";
		d_experiment->DeleteSpace(s->SpaceID());
	} catch ( const std::exception & e) {
		qCritical() << "Could not remove space '" << spaceName << "': " << e.what();
		return;
	}

	qInfo() << "Deleted Space:'" << spaceName << "'";
	d_model->removeRows(item->row(),1);

	setModified(true);
	emit spaceDeleted(spaceName);
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

	auto item = locateSpace(fi.first->Name().c_str());
	if ( item == NULL ) {
		return;
	}

	try {
		qDebug() << "[UniverseBridge]: Calling fort::myrmidon::priv::Experiment::DeleteTrackingDataDirectory("
		         << URI << "')";
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


QStandardItem * UniverseBridge::locateSpace(const QString & name) {
	auto items = d_model->findItems(name);
	QStandardItem  * item = nullptr;
	for ( const auto & i : items) {
		if ( i->data(Qt::UserRole+1) != SPACE_TYPE ) {
			continue;
		}
		if ( item != nullptr ) {
			qDebug() << "[UniverseBridge]: Could not locate Qt Item '" << name << "': multiple entry found";
		} else {
			item = i;
		}

	}
	if ( item == nullptr ) {
		qDebug() << "[UniverseBridge]: Could not locate Qt Item '" << name << "'";
	}
	return item;
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
	qDebug() << "[UniverseBride]: setting new experiment";
	d_experiment = experiment;
	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("URI"),
	                                    tr("Filepath"),
	                                    tr("Start FrameID"),
	                                    tr("End FrameID"),
	                                    tr("Start Date"),
	                                    tr("End Date")
		});
	setModified(false);

	if (!d_experiment) {
		emit activated(false);
		return;
	}
	rebuildAll(d_experiment->Spaces());
	emit activated(true);
}


bool UniverseBridge::isDeletable(const QModelIndexList & index) const {
	std::set<std::string> deleteNeeded;
	std::set<std::string> deleted;

	for ( const auto & i : index ) {
		auto item = d_model->itemFromIndex(i);
		if ( item == NULL ) {
			return false;
		}

		switch(item->data(Qt::UserRole+1).toInt()) {
		case SPACE_TYPE: {
			auto s = item->data(Qt::UserRole+2).value<fmp::Space::Ptr>();
			for ( const auto & tdd : s->TrackingDataDirectories() ) {
				deleteNeeded.insert(tdd->URI());
			}
			break;
		}
		case TDD_TYPE: {
			auto tdd = item->data(Qt::UserRole+2).value<fmp::TrackingDataDirectory::Ptr>();
			if ( d_experiment->TrackingDataDirectoryIsDeletable(tdd->URI()) == false ) {
				return false;
			}
			deleted.insert(tdd->URI());
			break;
		}
		}
	}

	for ( const auto & n : deleteNeeded ) {
		if ( deleted.count(n) == 0 ) {
			return false;
		}
	}

	return true;
}



void UniverseBridge::deleteSelection(const QModelIndexList & selection) {
	std::set<std::string> spaceURIs;
	std::set<std::string> tddURIs;
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
			spaceURIs.insert(item->data(Qt::UserRole+2).value<fmp::Space::Ptr>()->Name());
			break;
		case TDD_TYPE: {
			auto tdd = item->data(Qt::UserRole+2).value<fmp::TrackingDataDirectory::Ptr>();
			tddURIs.insert(tdd->URI());
			break;
		}
		}
	}

	for ( const auto & uri : tddURIs ) {
		deleteTrackingDataDirectory(uri.c_str());
	}
	for ( const auto & uri : spaceURIs ) {
		deleteSpace(uri.c_str());
	}
}

void UniverseBridge::rebuildAll(const fmp::SpaceByID & spaces) {
	for ( const auto & [spaceID,s] : spaces ) {
		d_model->appendRow(buildSpace(s));
	}
}


const fmp::Space::Universe::TrackingDataDirectoryByURI & UniverseBridge::trackingDataDirectories() const {
	if ( !d_experiment ) {
		static fmp::Space::Universe::TrackingDataDirectoryByURI empty;
		return empty;
	}
	return d_experiment->TrackingDataDirectories();
}
