#include "SelectedAntBridge.hpp"

#include "SelectedIdentificationBridge.hpp"

#include <QDebug>

#include <fort/studio/Format.hpp>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Capsule.hpp>

#include "IdentifierBridge.hpp"

SelectedAntBridge::SelectedAntBridge(IdentifierBridge * parent)
	: Bridge(parent)
	, d_identifier(parent)
	, d_identificationModel(new QStandardItemModel(this))
	, d_selectedIdentification( new SelectedIdentificationBridge(this) ) {

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::identificationModified,
	        this,
	        &SelectedAntBridge::onIdentificationModified);

}

bool SelectedAntBridge::isActive() const {
	return d_ant.get() != NULL;
}

void SelectedAntBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	d_experiment = experiment;
}


SelectedIdentificationBridge * SelectedAntBridge::selectedIdentification() const {
	return d_selectedIdentification;
}

QAbstractItemModel * SelectedAntBridge::identificationModel() const {
	return d_identificationModel;
}

const fmp::Ant::TypedCapsuleList & SelectedAntBridge::capsules() const {
	if ( !d_ant ) {
		static fmp::Ant::TypedCapsuleList empty;
		return empty;
	}
	return d_ant->Capsules();
}


void SelectedAntBridge::setAnt(const fmp::Ant::Ptr & ant) {
	setModified(false);
	d_identificationModel->clear();
	d_ant = ant;
	d_selectedIdentification->setIdentification(fmp::Identification::Ptr());
	if ( !d_ant ) {
		emit activated(false);
		return;
	}
	qInfo() << "Selected Ant " << fmp::Ant::FormatID(ant->ID()).c_str();
	rebuildIdentificationModel();
	emit activated(true);
}

void SelectedAntBridge::onIdentificationModified(const fmp::Identification::ConstPtr & ident) {
	if ( !d_ant || !ident || ident->Target()->ID() != d_ant->ID() ) {
		return;
	}
	rebuildIdentificationModel();
	setModified(true);
}


void SelectedAntBridge::rebuildIdentificationModel() {
	if ( !d_ant ) {
		return;
	}

	d_identificationModel->clear();
	d_identificationModel->setHorizontalHeaderLabels({tr("TagID"),tr("Size(mm)"),tr("From Time"),tr("To Time")});

	for ( const auto & ident : d_ant->Identifications() ) {
		auto data = QVariant::fromValue(ident);
		auto tag = new QStandardItem(QString::number(ident->TagValue()));
		tag->setEditable(false);
		tag->setData(data);
		double sizeValue = ident->TagSize();
		if ( sizeValue == 0.0 ) {
			sizeValue = d_experiment->DefaultTagSize();
		}
		auto size = new QStandardItem(QString::number(sizeValue));
		size->setEditable(false);
		size->setData(data);
		auto start = new QStandardItem("");
		start->setEditable(false);
		start->setData(data);
		if ( ident->Start() ) {
			start->setText(ToQString(*ident->Start()));
		} else {
			start->setText("-∞");
		}
		auto end = new QStandardItem("");
		end->setEditable(false);
		end->setData(data);
		if ( ident->End() ) {
			end->setText(ToQString(*ident->End()));
		} else {
			end->setText("+∞");
		}

		d_identificationModel->appendRow({tag,size,start,end});
	}
}


fm::Ant::ID SelectedAntBridge::selectedID() const {
	if ( !d_ant ) {
		return 0;
	}
	return d_ant->ID();
}


void SelectedAntBridge::selectIdentification(const QModelIndex & index) {
	auto item = d_identificationModel->itemFromIndex(index);
	if ( item == nullptr ) {
		return;
	}
	d_selectedIdentification->setIdentification(item->data().value<fmp::Identification::Ptr>());
}


void SelectedAntBridge::removeIdentification(const QModelIndex & index) {
	auto item = d_identificationModel->itemFromIndex(index);
	if ( item == nullptr || d_identifier == nullptr) {
		return;
	}


	d_identifier->deleteIdentification(item->data().value<fmp::Identification::Ptr>());
}


void SelectedAntBridge::addCapsule(fmp::AntShapeTypeID typeID,const fmp::CapsulePtr & capsule) {
	if ( !d_ant || !d_experiment || !capsule) {
		return;
	}
	try {
		qDebug() << "[SelectedAntBridge]: Calling fmp::Experiment::AddCapsuleToAnt("
		         << ToQString(fmp::Ant::FormatID(d_ant->ID()))
		         << "," << typeID
		         << "," << ToQString(*capsule)
		         << ")";
		d_ant->AddCapsule(typeID,capsule);
	} catch (const std::exception & e ) {
		qCritical() << "Could not add Capsule of type " << typeID
		            << " to Ant " << ToQString(fmp::Ant::FormatID(d_ant->ID()))
		            << ": " << e.what();
		return;
	}
	setModified(true);
}

void SelectedAntBridge::clearCapsules() {
	if ( !d_ant ) {
		return;
	}

	try {
		qDebug() << "[SelectedAntBridge]: Calling fmp::Ant("
		         << ToQString(fmp::Ant::FormatID(d_ant->ID()))
		         << ")::DeleteCapsules()";
		d_ant->ClearCapsules();
	} catch ( const std::exception & e ) {
		qCritical() << "Could not remove Capsules "
		            << " from Ant " << ToQString(fmp::Ant::FormatID(d_ant->ID()))
		            << ": " << e.what();
		return;
	}

	setModified(true);
}

std::vector<fmp::Identification::ConstPtr> SelectedAntBridge::identifications() const {
	if ( !d_ant ) {
		return {};
	}
	std::vector<fmp::Identification::ConstPtr> res;
	res.reserve(d_ant->Identifications().size());
	for ( const auto & i : d_ant->Identifications() ) {
		res.push_back(i);
	}
	return res;
}

void SelectedAntBridge::cloneShape(bool scaleToSize, bool overwriteShape) {
	if ( !d_ant == true || !d_experiment == true ) {
		return;
	}

	try {
		qDebug() << "[SelectedAntBridge]: Calling fmp::Experiment::CloneAntShape("
		         << d_ant->FormattedID().c_str() << ","
		         << scaleToSize << ","
		         << overwriteShape << ")";
		d_experiment->CloneAntShape(d_ant->ID(),
		                            scaleToSize,
		                            overwriteShape);
	} catch ( const std::exception & e ) {
		qCritical() << "Could not clone Ant shape " << e.what();
		return;
	}
	qInfo() << "Cloned Ant's " << d_ant->FormattedID().c_str() << " Shape.";
	setModified(true);

}
