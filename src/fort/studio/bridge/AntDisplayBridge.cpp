#include "AntDisplayBridge.hpp"

#include <QDebug>
#include <QItemSelection>

#include <fort/myrmidon/priv/Identifier.hpp>

#include <fort/studio/MyrmidonTypes/Conversion.hpp>

#include "ExperimentBridge.hpp"
#include "IdentifierBridge.hpp"


AntDisplayBridge::AntDisplayBridge(QObject * parent)
	: GlobalBridge(parent)
	, d_model(new QStandardItemModel(this))
	, d_numberSoloAnt(0)
	, d_numberHiddenAnt(0) {

	qRegisterMetaType<fmp::Ant::DisplayState>();
	qRegisterMetaType<fm::Color>();

	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &AntDisplayBridge::onAntItemChanged);


}

AntDisplayBridge::~AntDisplayBridge() {
}

QAbstractItemModel * AntDisplayBridge::model() const {
	return d_model;
}

void AntDisplayBridge::initialize(ExperimentBridge * experiment) {
}

void AntDisplayBridge::tearDownExperiment() {
	d_numberSoloAnt = 0;
	d_numberHiddenAnt = 0;
	clear();
}

void AntDisplayBridge::setUpExperiment() {
	emit numberSoloAntChanged(d_numberSoloAnt);
	emit numberHiddenAntChanged(d_numberHiddenAnt);
	rebuildModel();
}

void AntDisplayBridge::onAntCreated(quint32 antID ) {
	if ( !d_experiment ) {
		return;
	}
	try {
		auto ant = d_experiment->Identifier()->Ants().at(antID);
		d_model->insertRow(d_model->rowCount(),buildAnt(ant));
	} catch ( const std::exception & e ) {
	}
}

void AntDisplayBridge::onAntDeleted(quint32 antID ) {
	auto item = findAntItem(antID);
	if ( item == nullptr) {
		return;
	}
	d_model->removeRows(item->row(),1);
}


QList<QStandardItem*> AntDisplayBridge::buildAnt(const fmp::Ant::Ptr & ant) {
	auto data = QVariant::fromValue(ant);
	auto label = new QStandardItem(ExperimentBridge::formatAntName(ant));
	label->setEditable(false);
	label->setData(data);
	label->setData(antDisplayColor(ant),Qt::DecorationRole);

	auto hidden = new QStandardItem("");
	hidden->setCheckable(true);
	hidden->setData(data);
	hidden->setData(tr("Hide this ant in visualization"),Qt::ToolTipRole);
	hidden->setData(tr("Do not show this Ant in visualization."),Qt::WhatsThisRole);
	auto solo = new QStandardItem("");
	solo->setCheckable(true);
	solo->setData(data);
	solo->setData(tr("Solo this ant in visualization"),Qt::ToolTipRole);
	solo->setData(tr("When some Ant are Solo-ed, only this Ant would be displayed in visualization."),Qt::WhatsThisRole);
	switch(ant->DisplayStatus()) {
	case fmp::Ant::DisplayState::VISIBLE:
		hidden->setCheckState(Qt::Unchecked);
		solo->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::HIDDEN:
		++d_numberHiddenAnt;
		hidden->setCheckState(Qt::Checked);
		solo->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::SOLO:
		++d_numberSoloAnt;
		hidden->setCheckState(Qt::Unchecked);
		solo->setCheckState(Qt::Checked);
		break;
	}

	return {label,hidden,solo};
}

QIcon AntDisplayBridge::antDisplayColor(const fmp::Ant::ConstPtr & ant) {
	auto c = ant->DisplayColor();
	return Conversion::iconFromFM(c);
}


QStandardItem * AntDisplayBridge::findAntItem(fm::Ant::ID antID) const {
	auto items = d_model->findItems(fmp::Ant::FormatID(antID).c_str(), Qt::MatchStartsWith);
	if ( items.size() != 1 ) {
		qDebug() << "Could not find Ant " << fmp::Ant::FormatID(antID).c_str();
		return NULL;
	}
	return items[0];
}

fmp::Ant::Ptr AntDisplayBridge::findAnt(fm::Ant::ID antID) const {
	if ( !d_experiment ) {
		return nullptr;
	}
	try {
		return d_experiment->Identifier()->Ants().at(antID);
	} catch ( const std::exception & ) {
		return nullptr;
	}
}


void AntDisplayBridge::setAntDisplayState(QStandardItem * hideItem,
                                          QStandardItem * soloItem,
                                          const fmp::Ant::Ptr & ant,
                                          fmp::Ant::DisplayState ds) {
	auto oldDs = ant->DisplayStatus();
	if ( oldDs == ds ) {
		return;
	}
	ant->SetDisplayStatus(ds);
	switch(oldDs) {
	case fm::Ant::DisplayState::HIDDEN:
		--d_numberHiddenAnt;
		emit numberHiddenAntChanged(d_numberHiddenAnt);
		break;
	case fm::Ant::DisplayState::SOLO:
		--d_numberSoloAnt;
		emit numberSoloAntChanged(d_numberSoloAnt);
		break;
	case fm::Ant::DisplayState::VISIBLE:
		break;
	}

	switch(ds) {
	case fmp::Ant::DisplayState::VISIBLE:
		qInfo() << "Setting Ant " << ant->FormattedID().c_str()
		        << " to VISIBLE";
		hideItem->setCheckState(Qt::Unchecked);
		soloItem->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::HIDDEN:
		qInfo() << "Setting Ant " << ant->FormattedID().c_str()
		        << " to HIDDEN";
		++d_numberHiddenAnt;
		emit numberHiddenAntChanged(d_numberHiddenAnt);
		hideItem->setCheckState(Qt::Checked);
		soloItem->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::SOLO:
		qInfo() << "Setting Ant " << ant->FormattedID().c_str()
		        << " to SOLO";
		++d_numberSoloAnt;
		emit numberSoloAntChanged(d_numberSoloAnt);
		hideItem->setCheckState(Qt::Unchecked);
		soloItem->setCheckState(Qt::Checked);
		break;
	}
	setModified(true);
	emit antDisplayChanged(ant->AntID(),ant->DisplayColor(),ant->DisplayStatus());
}

void AntDisplayBridge::onAntItemChanged(QStandardItem * item) {
	if ( item->column() < HIDE_COLUMN || item->column() > SOLO_COLUMN ) {
		return;
	}

	auto ant = item->data().value<fmp::Ant::Ptr>();
	auto hideItem = d_model->item(item->row(),HIDE_COLUMN);
	auto soloItem = d_model->item(item->row(),SOLO_COLUMN);
	switch ( item->column() ) {
	case HIDE_COLUMN:
		if ( item->checkState() == Qt::Checked ){
			setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::HIDDEN);
		} else if (soloItem->checkState() == Qt::Unchecked ) {
			setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
		}
		break;
	case SOLO_COLUMN:
		if ( item->checkState() == Qt::Checked ) {
			setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::SOLO);
		} else if ( hideItem->checkState() == Qt::Unchecked) {
			setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
		}
		break;
	}

}

void AntDisplayBridge::doOnSelection(const QItemSelection & selection,
                                     const std::function<void (const fmp::Ant::Ptr & ant,
                                                               QStandardItem * item)> & toDo) {
	for ( const auto & index : selection.indexes() ) {
		if ( index.isValid() == false || index.column() != 0 ) {
			continue;
		}
		auto item = d_model->itemFromIndex(index);
		auto ant = item->data().value<fmp::Ant::Ptr>();
		if ( !ant ) {
			continue;
		}
		toDo(ant,item);
	}
}


quint32 AntDisplayBridge::numberHiddenAnt() const {
	return d_numberHiddenAnt;
}

quint32 AntDisplayBridge::numberSoloAnt() const {
	return d_numberSoloAnt;
}

void AntDisplayBridge::showAll() {
	for(size_t i = 0; i < d_model->rowCount(); ++i) {
		auto hideItem = d_model->itemFromIndex(d_model->index(i,HIDE_COLUMN));
		auto soloItem = d_model->itemFromIndex(d_model->index(i,SOLO_COLUMN));
		auto ant = hideItem->data().value<fmp::Ant::Ptr>();
		setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
	}
}

void AntDisplayBridge::unsoloAll() {
	for(size_t i = 0; i < d_model->rowCount(); ++i) {
		auto hideItem = d_model->itemFromIndex(d_model->index(i,HIDE_COLUMN));
		auto soloItem = d_model->itemFromIndex(d_model->index(i,SOLO_COLUMN));
		auto ant = hideItem->data().value<fmp::Ant::Ptr>();
		if (ant->DisplayStatus() != fmp::Ant::DisplayState::SOLO ) {
			continue;
		}
		setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
	}
}


void AntDisplayBridge::clear() {
	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("AntID"),tr("H"),tr("S")});
}

void AntDisplayBridge::rebuildModel() {
	clear();
	if ( isActive() == false ) {
		return;
	}
	for ( const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		d_model->insertRow(d_model->rowCount(),buildAnt(ant));
	}
}

void AntDisplayBridge::setAntDisplayColor(quint32 antID,
                                          const QColor & color) {
	auto item = findAntItem(antID);
	auto ant = findAnt(antID);
	if ( color.isValid() == false
	     || item == nullptr
	     || ant == nullptr ) {
		return;
	}

	qInfo() << "Setting Display Color of Ant " << ant->FormattedID().c_str()
	        << " to " << color;
	ant->SetDisplayColor({color.red(),color.green(),color.blue()});
	item->setData(antDisplayColor(ant),Qt::DecorationRole);

	setModified(true);
	emit antDisplayChanged(ant->AntID(),ant->DisplayColor(),ant->DisplayStatus());
}

fm::Ant::ID AntDisplayBridge::antIDForIndex(const QModelIndex & index) const {
	auto item = d_model->itemFromIndex(index);
	if ( item == nullptr ) {
		return 0;
	}
	return item->data().value<fmp::Ant::Ptr>()->AntID();
}


std::pair<fmp::Ant::DisplayState,fm::Color>
AntDisplayBridge::displayStatusAndColor(fm::Ant::ID antID) const {
	auto ant = findAnt(antID);
	if ( ant == nullptr ) {
		return {fmp::Ant::DisplayState::HIDDEN,fm::Color(0,0,0)};
	}
	return { ant->DisplayStatus(),ant->DisplayColor()};
}
