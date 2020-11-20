#include "TagCloseUpBridge.hpp"

#include <QStandardItemModel>

#include "IdentifierBridge.hpp"
#include "UniverseBridge.hpp"

#include <fort/myrmidon/priv/Identifier.hpp>

TagCloseUpBridge::TagCloseUpBridge(QObject * parent)
	: Bridge(parent)
	, d_tagModel(new QStandardItemModel(this)) {

	qRegisterMetaType<fmp::TagCloseUp::ConstPtr>();

	d_tagModel->setSortRole(Qt::UserRole+2);

}

TagCloseUpBridge::~TagCloseUpBridge() {
}

void TagCloseUpBridge::clear() {
	d_tagModel->clear();
	d_tagsLists.clear();
	d_antsLists.clear();
	emit cleared();
}

void TagCloseUpBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	clear();

	d_experiment = experiment;

	rebuild();

	emit activated(!d_experiment == false);
}

void TagCloseUpBridge::setUp(IdentifierBridge * identifier,
                             UniverseBridge * universe) {
	connect(identifier,
	        &IdentifierBridge::identificationCreated,
	        this,
	        &TagCloseUpBridge::onIdentificationModified);

	connect(identifier,
	        &IdentifierBridge::identificationRangeModified,
	        this,
	        &TagCloseUpBridge::onIdentificationModified);

	connect(identifier,
	        &IdentifierBridge::identificationDeleted,
	        this,
	        &TagCloseUpBridge::onIdentificationModified);

	connect(universe,
	        &UniverseBridge::trackingDataDirectoryAdded,
	        this,
	        &TagCloseUpBridge::onTrackingDataDirectoryAdded);

	connect(universe,
	        &UniverseBridge::trackingDataDirectoryDeleted,
	        this,
	        &TagCloseUpBridge::onTrackingDataDirectoryDeleted);

}

QAbstractItemModel * TagCloseUpBridge::tagModel() const {
	return d_tagModel;
}

static QVector<fmp::TagCloseUp::ConstPtr> empty;

const QVector<fmp::TagCloseUp::ConstPtr> &
TagCloseUpBridge::closeUpsForIndex(const QModelIndex & index) const {
	auto item = d_tagModel->itemFromIndex(index);
	if ( item ==  nullptr ) {
		return empty;
	}
	return closeUpsForTag(item->data(Qt::UserRole+1).toInt());
}


const QVector<fmp::TagCloseUp::ConstPtr> & TagCloseUpBridge::closeUpsForTag(fm::TagID tagID) const {
	auto fi = d_tagsLists.find(tagID);
	if ( fi == d_tagsLists.cend() ) {
		return empty;
	}
	return fi->second;
}

const QVector<fmp::TagCloseUp::ConstPtr> & TagCloseUpBridge::closeUpsForAnt(fm::AntID antID) const {
	auto fi = d_antsLists.find(antID);
	if ( fi == d_antsLists.cend() ) {
		return empty;
	}
	return fi->second;
}

std::pair<fm::TagID,fm::AntID>
TagCloseUpBridge::addCloseUp(const fmp::TagCloseUp::ConstPtr & closeUp) {

	fm::TagID tagID = closeUp->TagValue();
	fm::AntID antID = 0;

	d_tagsLists[tagID].push_back(closeUp);
	if ( d_experiment ) {
		auto identification = d_experiment->CIdentifier().Identify(closeUp->TagValue(),
		                                                           closeUp->Frame().Time());
		if ( identification ) {
			antID = identification->Target()->AntID();
			d_antsLists[antID].push_back(closeUp);
		}
	}
	const char * tagLabel = fm::FormatTagID(tagID).c_str();
	auto items = d_tagModel->findItems(tagLabel);
	if ( items.isEmpty() == false ) {
		return {tagID,antID};
	}

	auto tagItem = new QStandardItem(tagLabel);

	auto countItem = new QStandardItem("0");

	auto usedItem = new QStandardItem("0");

	QList<QStandardItem*> row = {tagItem,countItem,usedItem};
	for ( const auto & item : row ) {
		item->setEditable(false);
		item->setData(tagID,Qt::UserRole+1);
	}

	d_tagModel->insertRow(d_tagModel->rowCount(),row);
	return {tagID,antID};
}


std::pair<std::set<fmp::TagID>,std::set<fmp::AntID>>
TagCloseUpBridge::addTrackingDataDirectory(const fmp::TrackingDataDirectory::Ptr & tdd) {
	std::set<fm::TagID> tags;
	std::set<fm::AntID> ants;

	for (const auto & tcu : tdd->TagCloseUps() ) {
		const auto & [tagID,antID] = addCloseUp(tcu);
	}
	ants.erase(0);
	return {tags,ants};
}

void TagCloseUpBridge::onTrackingDataDirectoryAdded(const fmp::TrackingDataDirectory::Ptr & tdd) {
	const auto & [tags,ants] = addTrackingDataDirectory(tdd);
	for ( const auto & t : tags ) {
		sort(d_tagsLists[t]);
		count(t);
		emit closeUpsForTagChanged(t,d_tagsLists[t]);
	}

	for ( const auto & a : ants ) {
		sort(d_antsLists[a]);
		emit closeUpsForAntChanged(a,d_antsLists[a]);
	}
	d_tagModel->sort(0);
}

std::pair<std::set<fmp::TagID>,std::set<fmp::AntID>>
TagCloseUpBridge::removeTrackingDataDirectory(const QString & uri) {
	std::set<fm::TagID> tags;
	std::set<fm::AntID> ants;

	for ( auto & [tagID,tcus] : d_tagsLists ) {
		auto fi = std::remove_if(tcus.begin(),
		                         tcus.end(),
		                         [uri](const fmp::TagCloseUp::ConstPtr & tcu ) -> bool {
			                         return tcu->Frame().ParentURI().c_str() == uri;
		                         });
		if ( fi != tcus.end() ) {
			tcus.erase(fi,tcus.end());
			tags.insert(tagID);
		}
	}

	for ( auto & [antID,tcus] : d_antsLists ) {
		auto fi = std::remove_if(tcus.begin(),
		                         tcus.end(),
		                         [uri](const fmp::TagCloseUp::ConstPtr & tcu ) -> bool {
			                         return tcu->Frame().ParentURI().c_str() == uri;
		                         });

		if ( fi != tcus.end() ) {
			tcus.erase(fi,tcus.end());
			ants.insert(antID);
		}
	}

	return {tags,ants};
}




void TagCloseUpBridge::onTrackingDataDirectoryDeleted(const QString & uri) {
	const auto & [tags,ants] = removeTrackingDataDirectory(uri);
	for ( const auto & t : tags ) {
		count(t);
		emit closeUpsForTagChanged(t,d_tagsLists[t]);
	}

	for ( const auto & a : ants ) {
		emit closeUpsForAntChanged(a,d_antsLists[a]);
	}
	d_tagModel->sort(0);
}

void TagCloseUpBridge::onIdentificationModified(fmp::Identification::ConstPtr identification) {
	count(identification->TagValue());

	auto ant = identification->Target();
	auto & antList = d_antsLists[ant->AntID()];
	antList.clear();
	for ( const auto & identification : ant->Identifications() ) {
		for ( const auto & tcu : d_tagsLists[identification->TagValue()] ) {
			if ( identification->IsValid(tcu->Frame().Time()) == true ) {
				antList.push_back(tcu);
			}
		}
	}
	sort(d_antsLists[ant->AntID()]);
	emit closeUpsForAntChanged(ant->AntID(),antList);
}

void TagCloseUpBridge::count(fm::TagID tagID) {
	const auto & closeUps = d_tagsLists[tagID];
	size_t total = closeUps.size();
	size_t used = 0;
	if ( d_experiment ) {
		const auto & identifier = d_experiment->CIdentifier();
		used = std::count_if(closeUps.begin(),
		                     closeUps.end(),
		                     [&identifier](const fmp::TagCloseUp::ConstPtr & tcu) -> bool {
			                     return identifier.Identify(tcu->TagValue(),tcu->Frame().Time()) != nullptr;
		                     });
	}

	auto items = d_tagModel->findItems(fm::FormatTagID(tagID).c_str());
	if ( items.isEmpty() == true ) {
		return;
	}
	auto row = items[0]->row();

	if ( total != 0 ) {

		d_tagModel->item(row,1)->setText(QString::number(total));
		d_tagModel->item(row,2)->setText(QString::number(used));

	} else {
		d_tagsLists.erase(tagID);
		d_tagModel->removeRow(row);
	}
}

void TagCloseUpBridge::sort(QVector<fmp::TagCloseUp::ConstPtr> & tcus) {
	std::sort(tcus.begin(),tcus.end(),
	          [](const fmp::TagCloseUp::ConstPtr & a,
	             const fmp::TagCloseUp::ConstPtr & b) -> bool {
		          if ( !a && !b ) { return false;}
		          if ( a && b ) {
			          return a->Frame().Time().Before(b->Frame().Time());
		          }
		          return !b;
	          });
}


void TagCloseUpBridge::rebuild() {
	d_tagModel->setHorizontalHeaderLabels({tr("TagID"),tr("Count"),tr("Used")});
	if ( !d_experiment == true ) {
		return;
	}
	for ( const auto & [tddURi,tdd] : d_experiment->TrackingDataDirectories() ) {
		addTrackingDataDirectory(tdd);
	}

	for ( auto & [t,tcus] : d_tagsLists ) {
		sort(tcus);
		count(t);
		emit closeUpsForTagChanged(t,tcus);
	}

	for ( auto & [a,tcus] : d_antsLists ) {
		sort(tcus);
		emit closeUpsForAntChanged(a,tcus);
	}
	d_tagModel->sort(0);
}

bool TagCloseUpBridge::isActive() const {
	return !d_experiment == false;
}


QModelIndex TagCloseUpBridge::indexForTag(fm::TagID tagID) const {
	auto items =  d_tagModel->findItems(fmp::FormatTagID(tagID).c_str());
	if ( items.isEmpty() ) {
		return QModelIndex();
	}
	return items.front()->index();
}
