#include "IdentifierBridge.hpp"

#include <QDebug>
#include <QItemSelection>

#include "Format.hpp"
#include "ColorComboBox.hpp"

#include <myrmidon/priv/Identifier.hpp>


IdentifierBridge::IdentifierBridge(QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this)) {

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
		return;
	}
	d_experiment->Identifier()
		.SetAntPositionUpdateCallback([=](const fmp::Identification::Ptr & ident) {
			                              qDebug() << "Got ant position update for " << ToQString(ident);
			                              emit identificationAntPositionModified(ident);
		                              });

	for ( const auto & [AID,a] : d_experiment->Identifier().Ants() ) {
		d_model->invisibleRootItem()->appendRow(buildAnt(a));
	}

	emit activated(true);
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
		hidden->setCheckState(Qt::Checked);
		solo->setCheckState(Qt::Unchecked);
		break;
	case fmp::Ant::DisplayState::SOLO:
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


void IdentifierBridge::onItemChanged(QStandardItem * item) {
	if ( item->column() < HIDE_COLUMN || item->column() > SOLO_COLUMN ) {
		return;
	}

	auto ant = item->data().value<fmp::Ant::Ptr>();
	switch ( item->column() ) {
	case HIDE_COLUMN:
		if ( item->checkState() == Qt::Checked ){
			qInfo() << "Setting Ant " << fmp::Ant::FormatID(ant->ID()).c_str()
			        << " to HIDDEN";
			ant->SetDisplayStatus(fmp::Ant::DisplayState::HIDDEN);
			d_model->item(item->row(),SOLO_COLUMN)->setCheckState(Qt::Unchecked);
			setModified(true);
			emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
		} else if (d_model->item(item->row(),SOLO_COLUMN)->checkState() == Qt::Unchecked ) {
			qInfo() << "Setting Ant " << fmp::Ant::FormatID(ant->ID()).c_str()
			        << " to VISIBLE";
			ant->SetDisplayStatus(fmp::Ant::DisplayState::VISIBLE);
			setModified(true);
			emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
		}
		break;
	case SOLO_COLUMN:
		if ( item->checkState() == Qt::Checked ) {
			qInfo() << "Setting Ant " << fmp::Ant::FormatID(ant->ID()).c_str()
			        << " to SOLO";
			ant->SetDisplayStatus(fmp::Ant::DisplayState::SOLO);
			d_model->item(item->row(),HIDE_COLUMN)->setCheckState(Qt::Unchecked);
			setModified(true);
			emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
		} else if ( d_model->item(item->row(),HIDE_COLUMN)->checkState() == Qt::Unchecked) {
			qInfo() << "Setting Ant " << fmp::Ant::FormatID(ant->ID()).c_str()
			        << " to VISIBLE";
			ant->SetDisplayStatus(fmp::Ant::DisplayState::VISIBLE);
			setModified(true);
			emit antDisplayChanged(ant->ID(),ant->DisplayColor(),ant->DisplayStatus());
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

	emit antSelected(ant);
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
