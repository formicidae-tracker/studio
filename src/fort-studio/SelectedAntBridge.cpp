#include "SelectedAntBridge.hpp"


SelectedAntBridge::SelectedAntBridge(QObject * parent)
	: QObject(parent)
	, d_identificationModel(new QStandardItemModel(this))
	, d_shapeModel(new QStandardItemModel(this)) {
}

QAbstractItemModel * SelectedAntBridge::identificationModel() const {
	return d_identificationModel;
}

QAbstractItemModel * SelectedAntBridge::shapeModel() const {
	return d_shapeModel;
}

void SelectedAntBridge::setAnt(const fmp::Ant::Ptr & ant) {
	d_identificationModel->clear();
	d_shapeModel->clear();
	d_ant = ant;
	if ( !d_ant ) {
		emit activeStateChanged(false);
		return;
	}
	rebuildIdentificationModel();
	emit activeStateChanged(true);
}

void SelectedAntBridge::onIdentificationModified(const fmp::Identification::ConstPtr & ident) {
	if ( d_ant || ident->Target() != d_ant ) {
		return;
	}
	rebuildIdentificationModel();
}


void SelectedAntBridge::rebuildIdentificationModel() {
	if ( !d_ant ) {
		return;
	}

	d_identificationModel->clear();

	for ( const auto & ident : d_ant->Identifications() ) {
		auto data = QVariant::fromValue(ident);
		auto tag = new QStandardItem(QString::number(ident->TagValue()));
		tag->setEditable(false);
		tag->setData(data);
		auto start = new QStandardItem("");
		start->setEditable(false);
		start->setData(data);
		if ( ident->Start() ) {
			start->setText(ident->Start()->DebugString().c_str());
		} else {
			start->setText("-∞");
		}
		auto end = new QStandardItem("");
		end->setEditable(false);
		end->setData(data);
		if ( ident->End() ) {
			end->setText(ident->End()->DebugString().c_str());
		} else {
			end->setText("+∞");
		}

		d_identificationModel->appendRow({tag,start,end});
	}
}
