#include "AntMetadataBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/Format.hpp>

AntMetadataBridge::AntMetadataBridge(QObject * parent)
	: Bridge(parent)
	, d_columnModel(new QStandardItemModel(this)) {
	qRegisterMetaType<fmp::AntMetadata::Column::Ptr>();
}

AntMetadataBridge::~AntMetadataBridge() {
}

bool AntMetadataBridge::isActive() const {
	return !d_experiment == false;
}

void AntMetadataBridge::setExperiment(const fmp::Experiment::Ptr & experiment) {
	setModified(false);
	d_columnModel->clear();
	d_columnModel->setHorizontalHeaderLabels({tr("Name"),tr("Type"),tr("Default Value")});

	d_experiment = experiment;
	if ( !d_experiment ) {
		emit activated(false);
		return;
	}

	emit activated(true);
}

QAbstractItemModel * AntMetadataBridge::columnModel() {
	return d_columnModel;
}

QAbstractItemModel * AntMetadataBridge::dataModel() {
	return nullptr;
}

void AntMetadataBridge::addMetadataColumn(const QString & name, quint32 type) {
	if ( !d_experiment ) {
		return;
	}
	fmp::AntMetadata::Column::Ptr column;
	try {
		qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::Experiment::AddAntMetadataColumn("
		         << name << "," << type;
		column = d_experiment->AddAntMetadataColumn(ToStdString(name),fmp::AntMetadata::Type(type));
	} catch ( const std::exception & e) {
		qCritical() << "Could not create AntMetadataColumn '" << name << "':" << e.what();
		return;
	}

	qInfo() << "Created new AntMetadataColumn " << name;
	d_columnModel->appendRow(buildColumn(column));
	d_columnModel->sort(0,Qt::AscendingOrder);
	setModified(true);
	emit metadataColumnChanged(name,type);
}

void AntMetadataBridge::removeMetadataColumn(const QString & name) {
	if ( !d_experiment ) {
		return;
	}
	auto items = d_columnModel->findItems(name);
	if ( items.isEmpty() == true ) {
		qWarning() << "Not removing AntMetadataColum " << name << ": could not find item in internal model";
		return;
	}
	auto item = items[0];

	try {
		qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::Experiment::DeleteAntMetadataColumn("
		         << name << ")";
		d_experiment->DeleteAntMetadataColumn(ToStdString(name));
	} catch ( const std::exception & e ) {
		qCritical() << "Could not delete AntMetadataColumn " << name
		            << ": " << e.what();
		return;
	}

	qInfo() << "Deleted AntMetadataColumn " << name;

	d_columnModel->removeRows(item->row(),1);

	setModified(true);
	emit metadataColumnRemoved(name);
}


QList<QStandardItem*> AntMetadataBridge::buildColumn(const fmp::AntMetadata::Column::Ptr & column) {
	auto data = QVariant::fromValue(column);
	auto nameItem = new QStandardItem(ToQString(column->Name()));
	nameItem->setData(data);

	auto typeItem = new QStandardItem(QString::number(quint32(column->MetadataType())));
	typeItem->setData(data);

	std::ostringstream oss;
	std::visit([&oss](auto && arg ) {
		           oss << std::boolalpha << "'" << arg << "'";
	           },
		fmp::AntMetadata::DefaultValue(column->MetadataType()));


	auto valueItem = new QStandardItem(oss.str().c_str());
	valueItem->setEditable(false);
	valueItem->setData(data);

	return {nameItem,typeItem,valueItem};
}
