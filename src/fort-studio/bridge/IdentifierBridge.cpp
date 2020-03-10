#include "IdentifierBridge.hpp"

#include <QDebug>
#include <QItemSelection>

#include <fort-studio/Format.hpp>
#include <fort-studio/widget/base/ColorComboBox.hpp>

#include <myrmidon/priv/Identifier.hpp>

#include "SelectedAntBridge.hpp"

IdentifierBridge::IdentifierBridge(SelectedAntBridge * selectedAnt,
                                   QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this))
	, d_numberSoloAnt(0)
	, d_numberHiddenAnt(0)
	, d_selectedAnt(selectedAnt) {

	qRegisterMetaType<fmp::Ant::ConstPtr>();
	qRegisterMetaType<fmp::Ant::Ptr>();
	qRegisterMetaType<fmp::Identification::ConstPtr>();
	qRegisterMetaType<fmp::Ant::DisplayState>();
	qRegisterMetaType<fmp::Color>();

	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &IdentifierBridge::onItemChanged);

}

bool IdentifierBridge::isActive() const {
	return d_experiment.get() != NULL;
}


QAbstractItemModel * IdentifierBridge::antModel() const {
	return d_model;
}

void IdentifierBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	qDebug() << "[IdentifierBridge]: setting new experiment";
	d_numberSoloAnt = 0;
	d_numberHiddenAnt = 0;
	d_selectedAnt->setAnt(fmp::Ant::Ptr());

	setModified(false);
	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("Ant"),tr("H"),tr("S")});
	if ( d_experiment ) {
		d_experiment->Identifier()
			.SetAntPositionUpdateCallback([](const fmp::Identification::Ptr & i) {
			                              });
	}

	d_experiment = experiment;
	if ( !d_experiment ) {
		emit activated(false);
		emit numberSoloAntChanged(d_numberSoloAnt);
		emit numberHiddenAntChanged(d_numberHiddenAnt);
		return;
	}
	d_experiment->Identifier()
		.SetAntPositionUpdateCallback([=](const fmp::Identification::Ptr & ident) {
			                              qDebug() << "Got ant position update for " << ToQString(ident);
			                              emit identificationAntPositionModified(ident);
		                              });

	//reorder ants
	std::map<quint32,fmp::Ant::Ptr> ants;
	for ( const auto & a : d_experiment->Identifier().Ants() ) {
		ants.insert(a);
	}

	for ( const auto & [AID,a] : ants) {
		d_model->invisibleRootItem()->appendRow(buildAnt(a));
	}

	emit activated(true);
	emit numberSoloAntChanged(d_numberSoloAnt);
	emit numberHiddenAntChanged(d_numberHiddenAnt);
}

fmp::Ant::Ptr IdentifierBridge::createAnt() {
	if ( !d_experiment ) {
		return fmp::Ant::Ptr();
	}
	fmp::Ant::Ptr ant;
	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifier::CreateAnt()";
		ant = d_experiment->Identifier().CreateAnt();
	} catch ( const std::exception & e) {
		qCritical() << "Could not create Ant: " << e.what();
		return fmp::Ant::Ptr();
	}

	qInfo() << "Created new Ant" << fmp::Ant::FormatID(ant->ID()).c_str();

	d_model->invisibleRootItem()->appendRow(buildAnt(ant));

	setModified(true);
	emit antCreated(ant);
	return ant;
}

void IdentifierBridge::deleteAnt(fm::Ant::ID AID) {
	auto item = findAnt(AID);
	if ( !d_experiment || item == NULL) {
		qWarning() << "Not removing Ant " << fmp::Ant::FormatID(AID).c_str();
		return;
	}

	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifier::DeleteAnt("
		         << fmp::Ant::FormatID(AID).c_str() << ")";
		d_experiment->Identifier().DeleteAnt(AID);
	} catch (const std::exception & e) {
		qCritical() << "Could not delete Ant '" <<  fmp::Ant::FormatID(AID).c_str()
		            << "': " << e.what();
		return;
	}

	qInfo() << "Deleted Ant " << fmp::Ant::FormatID(AID).c_str();

	d_model->removeRows(item->row(),1);
	setModified(true);
	emit antDeleted(AID);
}


fmp::Identification::Ptr IdentifierBridge::addIdentification(fm::Ant::ID AID,
                                                             fmp::TagID TID,
                                                             const fm::Time::ConstPtr & start,
                                                             const fm::Time::ConstPtr & end) {

	auto item = findAnt(AID);

	if ( !d_experiment || item == NULL ) {
		qWarning() << "Not Adding Identification to Ant " << fmp::Ant::FormatID(AID).c_str();
		return fmp::Identification::Ptr();
	}

	fmp::Identification::Ptr identification;
	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifider::AddIdentification( "
		         << fmp::Ant::FormatID(AID).c_str()
		         << "," << TID
		         <<  "," << ToQString(start,"-")
		         << "," << ToQString(end,"+") << ")";
		identification = d_experiment->Identifier().AddIdentification(AID,TID,start,end);
	} catch (const std::exception & e) {
		qCritical() << "Could not create Identification " << fmp::Ant::FormatID(AID).c_str()
		            << "↤" << TID
		            << " [" << ToQString(start,"-")
		            << ";" << ToQString(end,"+")
		            << "]: " << e.what();
		return fmp::Identification::Ptr();
	}

	qInfo() << "Added Identification " << ToQString(identification);
	item->setText(formatAntName(item->data().value<fmp::Ant::Ptr>()));

	emit identificationCreated(identification);
	setModified(true);
	return identification;
}

void IdentifierBridge::deleteIdentification(const fmp::Identification::Ptr & identification) {
	auto item = findAnt(identification->Target()->ID());
	if ( !d_experiment || item == NULL) {
		qWarning() << "Not deleting Identification "
		           << ToQString(identification);
		return ;
	}

	try {
		qDebug() << "[IdentifierBridge]: Calling fort::myrmidon::priv::Identifier::DeleteIdentification("
		         << ToQString(identification) << ")";
		d_experiment->Identifier().DeleteIdentification(identification);
	} catch (const std::exception & e ) {
		std::ostringstream os;
		os << *identification;
		qCritical() << "Could not delete identification " << os.str().c_str()
		            << ": " <<  e.what();
		return;
	}

	qInfo() << "Deleted identification " << ToQString(identification);

	item->setText(formatAntName(item->data().value<fmp::Ant::Ptr>()));
	setModified(true);
	emit identificationDeleted(identification);
}

QString IdentifierBridge::formatAntName(const fmp::Ant::Ptr & ant) {
	QString res = fmp::Ant::FormatID(ant->ID()).c_str();
	if ( ant->Identifications().empty() ) {
		return res + " <no-tags>";
	}
	std::set<fmp::TagID> tags;
	for ( const auto & i : ant->Identifications() ) {
		tags.insert(i->TagValue());
	}
	QString prefix = " ↤ {";
	for ( const auto & t : tags ) {
		res += prefix + QString::number(t);
		prefix = ",";
	}
	return res + "}";
}

QList<QStandardItem*> IdentifierBridge::buildAnt(const fmp::Ant::Ptr & ant) {
	auto data = QVariant::fromValue(ant);
	auto label = new QStandardItem(formatAntName(ant));
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

QIcon IdentifierBridge::antDisplayColor(const fmp::Ant::Ptr & ant) {
	auto c = ant->DisplayColor();
	return ColorComboBox::iconFromColor(ColorComboBox::fromMyrmidon(c));
}

QString IdentifierBridge::formatIdentification(const fmp::Identification::Ptr & ident) {
	std::ostringstream os;
	os << "↤ " << ident->TagValue();
	if ( ident->Start() ) {
		os << "[" << ident->Start()->DebugString();
	} else {
		os << "]-∞";
	}
	if ( ident->End() ) {
		os << ";" << ident->End()->DebugString() << "[";
	} else {
		os << "+∞[";
	}
	return os.str().c_str();
}


QStandardItem * IdentifierBridge::findAnt(fm::Ant::ID AID) const {
	auto items = d_model->findItems(fmp::Ant::FormatID(AID).c_str(), Qt::MatchStartsWith);
	if ( items.size() != 1 ) {
		qDebug() << "Could not find Ant " << fmp::Ant::FormatID(AID).c_str();
		return NULL;
	}
	return items[0];
}

void IdentifierBridge::setAntDisplayState(QStandardItem * hideItem,
                                          QStandardItem * soloItem,
                                          const fmp::Ant::Ptr & ant,
                                          fmp::Ant::DisplayState ds) {
	auto oldDs = ant->DisplayStatus();
	if ( oldDs == ds ) {
		return;
	}
	ant->SetDisplayStatus(ds);
	switch(oldDs) {
	case fmp::Ant::DisplayState::HIDDEN:
		--d_numberHiddenAnt;
		emit numberHiddenAntChanged(d_numberHiddenAnt);
		break;
	case fmp::Ant::DisplayState::SOLO:
		--d_numberSoloAnt;
		emit numberSoloAntChanged(d_numberSoloAnt);
		break;
	}

	switch(ds) {
	case fmp::Ant::DisplayState::VISIBLE:
		qInfo() << "Setting Ant " << fmp::Ant::FormatID(ant->ID()).c_str()
		        << " to VISIBLE";
		hideItem->setCheckState(Qt::Unchecked);
		soloItem->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::HIDDEN:
		qInfo() << "Setting Ant " << fmp::Ant::FormatID(ant->ID()).c_str()
		        << " to HIDDEN";
		++d_numberHiddenAnt;
		emit numberHiddenAntChanged(d_numberHiddenAnt);
		hideItem->setCheckState(Qt::Checked);
		soloItem->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::SOLO:
		qInfo() << "Setting Ant " << fmp::Ant::FormatID(ant->ID()).c_str()
		        << " to SOLO";
		++d_numberSoloAnt;
		emit numberSoloAntChanged(d_numberSoloAnt);
		hideItem->setCheckState(Qt::Unchecked);
		soloItem->setCheckState(Qt::Checked);
		break;
	}
	setModified(true);
	emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
}

void IdentifierBridge::onItemChanged(QStandardItem * item) {
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


void IdentifierBridge::selectAnt(const QModelIndex & index) {
	if ( index.isValid() == false
	     || index.column() != 0 ) {
		qDebug() << "[IdentifierBridge]: Wrong column " <<  index.column() << " for selection";
		return;
	}

	auto ant = d_model->itemFromIndex(index)->data().value<fmp::Ant::Ptr>();
	d_selectedAnt->setAnt(ant);
}


void IdentifierBridge::doOnSelection(const QItemSelection & selection,
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


void IdentifierBridge::setAntDisplayColor(const QItemSelection & selection,
                                          const QColor & color) {
	if ( color.isValid() == false ) {
		return;
	}
	doOnSelection(selection,
	              [this,&color](const fmp::Ant::Ptr & ant,
	                            QStandardItem * item) {
		              qInfo() << "Setting Display Color of Ant " << fmp::Ant::FormatID(ant->ID()).c_str()
		                      << " to " << color;
		              ant->SetDisplayColor({color.red(),color.green(),color.blue()});

		              item->setData(antDisplayColor(ant),Qt::DecorationRole);

		              setModified(true);
		              emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
	              });
}


void IdentifierBridge::deleteSelection(const QItemSelection & selection) {
	std::set<fmp::Identification::Ptr> toDeleteIdentifications;
	std::set<fm::Ant::ID> toDeleteAID;
	doOnSelection(selection,
	              [&,this](const fmp::Ant::Ptr & ant,
	                     QStandardItem *) {
		              for( const auto & i : ant->Identifications() ) {
			              toDeleteIdentifications.insert(i);
		              }
		              toDeleteAID.insert(ant->ID());
	              });

	for ( const auto & i : toDeleteIdentifications ) {
		deleteIdentification(i);
	}
	for ( const auto & AID : toDeleteAID ) {
		deleteAnt(AID);
	}
}

quint32 IdentifierBridge::numberHiddenAnt() const {
	return d_numberHiddenAnt;
}

quint32 IdentifierBridge::numberSoloAnt() const {
	return d_numberSoloAnt;
}


void IdentifierBridge::showAll() {
	for(size_t i = 0; i < d_model->rowCount(); ++i) {
		auto hideItem = d_model->itemFromIndex(d_model->index(i,HIDE_COLUMN));
		auto soloItem = d_model->itemFromIndex(d_model->index(i,SOLO_COLUMN));
		auto ant = hideItem->data().value<fmp::Ant::Ptr>();
		setAntDisplayState(hideItem,soloItem,ant,fmp::Ant::DisplayState::VISIBLE);
	}
}

void IdentifierBridge::unsoloAll() {
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

fmp::Identification::ConstPtr IdentifierBridge::identify(fmp::TagID tagID,
                                                         const fm::Time & time) const {
	if ( !d_experiment ) {
		return fmp::Identification::ConstPtr();
	}
	return d_experiment->ConstIdentifier().Identify(tagID,time);
}


bool IdentifierBridge::freeRangeContaining(fm::Time::ConstPtr & start,
                                           fm::Time::ConstPtr & end,
                                           fmp::TagID tagID, const fm::Time & time) const {
	if ( !d_experiment ) {
		return false;
	}
	return d_experiment->ConstIdentifier().FreeRangeContaining(start,end,tagID,time);
}
