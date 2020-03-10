#include "SelectedAntBridge.hpp"

#include "SelectedIdentificationBridge.hpp"

#include <QDebug>

#include <fort-studio/Format.hpp>

#include <myrmidon/priv/Experiment.hpp>

SelectedAntBridge::SelectedAntBridge(QObject * parent)
	: Bridge(parent)
	, d_identificationModel(new QStandardItemModel(this))
	, d_shapeModel(new QStandardItemModel(this))
	, d_selectedIdentification( new SelectedIdentificationBridge(this) ){

	connect(d_selectedIdentification,
	        &SelectedIdentificationBridge::identificationModified,
	        this,
	        &SelectedAntBridge::onIdentificationModified);

}

bool SelectedAntBridge::isActive() const {
	return d_ant.get() != NULL;
}

void SelectedAntBridge::setExperiment(const fmp::Experiment::ConstPtr & experiment) {
	d_experiment = experiment;
}


SelectedIdentificationBridge * SelectedAntBridge::selectedIdentification() const {
	return d_selectedIdentification;
}

QAbstractItemModel * SelectedAntBridge::identificationModel() const {
	return d_identificationModel;
}

QAbstractItemModel * SelectedAntBridge::shapeModel() const {
	return d_shapeModel;
}

void SelectedAntBridge::setAnt(const fmp::Ant::Ptr & ant) {
	setModified(false);
	d_identificationModel->clear();
	d_shapeModel->clear();
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
	if ( !item ) {
		return;
	}
	d_selectedIdentification->setIdentification(item->data().value<fmp::Identification::Ptr>());
}
