#include "IdentifierBridge.hpp"

#include <QDebug>

#include <myrmidon/priv/Identifier.hpp>


IdentifierBridge::IdentifierBridge(QObject * parent)
	: QObject(parent)
	, d_model(new QStandardItemModel(this)) {
}

QAbstractItemModel * IdentifierBridge::antModel() const {
	return d_model;
}

void IdentifierBridge::SetExperiment(const fmp::Experiment::Ptr & experiment) {
	d_model->clear();

	if ( d_experiment ) {
		d_experiment->Identifier()
			.SetAntPositionUpdateCallback([](const fmp::Identification::Ptr & i) {
			                              });
	}

	d_experiment = experiment;
	if ( !d_experiment ) {
		return;
	}
	d_experiment->Identifier()
		.SetAntPositionUpdateCallback([=](const fmp::Identification::Ptr & ident) {
			                              emit identificationAntPositionModified(ident);
		                              });

	for ( const auto & [AID,a] : d_experiment->Identifier().Ants() ) {
		d_model->invisibleRootItem()->appendRow(buildAnt(a));
	}
}

fmp::Ant::Ptr IdentifierBridge::createAnt() {
	if ( !d_experiment ) {
		return fmp::Ant::Ptr();
	}
	fmp::Ant::Ptr ant;
	try {
		ant = d_experiment->Identifier().CreateAnt();
	} catch ( const std::exception & e) {
		qWarning() << "Could not create Ant: " << e.what();
		return fmp::Ant::Ptr();
	}

	d_model->invisibleRootItem()->appendRow(buildAnt(ant));
	emit antCreated(ant);
}

void IdentifierBridge::removeAnt(fm::Ant::ID AID) {
	auto item = findAnt(AID);
	if ( !d_experiment || item == NULL) {
		return;
	}


	try {
		d_experiment->Identifier().DeleteAnt(AID);
	} catch (const std::exception & e) {
		qWarning() << "Could not delete Ant '" <<  fmp::Ant::FormatID(AID).c_str()
		           << "': " << e.what();
		return;
	}

	d_model->removeRows(item->row(),1);
	emit antDeleted(AID);
}

fmp::Identification::Ptr IdentifierBridge::addIdentification(fm::Ant::ID AID,
                                                             fmp::TagID TID,
                                                             fm::Time::ConstPtr & start,
                                                             fm::Time::ConstPtr & end) {

	static auto formatTime= [](const fm::Time::ConstPtr & t, const QString & prefix) -> QString {
		                        if (t) {
			                        std::ostringstream os;
			                        os << t;
			                        return os.str().c_str();
		                        }
		                        return prefix + "∞";
	                        };

	auto item = findAnt(AID);

	if ( !d_experiment || item == NULL ) {
		return fmp::Identification::Ptr();
	}

	fmp::Identification::Ptr identification;
	try {
		identification = d_experiment->Identifier().AddIdentification(AID,TID,start,end);
	} catch (const std::exception & e) {
		qWarning() << "Could not create Identification " << fmp::Ant::FormatID(AID).c_str()
		           << " <- " << TID
		           << ", [" << formatTime(start,"-")
		           << ";" << formatTime(end,"+")
		           << "]: " << e.what();
		return fmp::Identification::Ptr();
	}

	rebuildIdentifications(item,item->data().value<fmp::Ant::Ptr>());
	emit identificationCreated(identification);

}

void IdentifierBridge::deleteIdentification(const fmp::Identification::Ptr & identification) {
	auto item = findAnt(identification->Target()->ID());
	if ( !d_experiment || item == NULL) {
		return ;
	}

	try {
		d_experiment->Identifier().DeleteIdentification(identification);
	} catch (const std::exception & e ) {
		std::ostringstream os;
		os << *identification;
		qWarning() << "Could not delete identification " << os.str().c_str()
		           << ": " <<  e.what();
		return;
	}

	rebuildIdentifications(item,item->data().value<fmp::Ant::Ptr>());

	emit identificationDeleted(identification);
}

QList<QStandardItem*> IdentifierBridge::buildAnt(const fmp::Ant::Ptr & ant) {
	auto item = new QStandardItem(fmp::Ant::FormatID(ant->ID()).c_str());
	item->setEditable(false);
	item->setData(QVariant::fromValue(ant));
	rebuildIdentifications(item,ant);
	return {item};
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

QList<QStandardItem*> IdentifierBridge::buildIdentification(const fmp::Identification::Ptr & ident) {
	auto item = new QStandardItem(formatIdentification(ident));
	item->setEditable(false);
	item->setData(QVariant::fromValue(ident));
	return {item};
}

void IdentifierBridge::rebuildIdentifications(QStandardItem * toItem,
                                              const fmp::Ant::Ptr & ant) {
	toItem->removeRows(0,toItem->rowCount());
	for (const auto & identification : ant->Identifications() ) {
		toItem->appendRow(buildIdentification(identification));
	}
}

QStandardItem * IdentifierBridge::findAnt(fm::Ant::ID AID) const {
	auto items = d_model->findItems(fmp::Ant::FormatID(AID).c_str());
	if ( items.size() != 1 ) {
		return NULL;
	}
	return items[0];
}
