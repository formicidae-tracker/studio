#include "AntShapeTypeBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/AntShapeType.hpp>

#include <fort/studio/Format.hpp>


AntShapeTypeBridge::AntShapeTypeBridge(QObject * parent)
	: Bridge(parent)
	, d_model(new QStandardItemModel(this) ) {
	qRegisterMetaType<fmp::AntShapeType::Ptr>();

	connect(d_model,
	        &QStandardItemModel::itemChanged,
	        this,
	        &AntShapeTypeBridge::onTypeItemChanged);
}

AntShapeTypeBridge::~AntShapeTypeBridge() {
}

bool AntShapeTypeBridge::isActive() const {
	return d_experiment.get () != nullptr;
}

void AntShapeTypeBridge::setExperiment(const fmp::ExperimentPtr & experiment) {
	setModified(false);
	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("Name"),tr("TypeID")});
	d_experiment = experiment;

	if ( !d_experiment ) {
		emit activated(false);
		return;
	}

	emit activated(true);

	for ( const auto & [typeID,shapeType] : d_experiment->AntShapeTypes() ) {
		d_model->appendRow(buildTypeItem(shapeType));
	}

}

QAbstractItemModel * AntShapeTypeBridge::shapeModel() const {
	return d_model;
}

void AntShapeTypeBridge::addType(const QString & name) {
	if ( !d_experiment ) {
		return;
	}
	fmp::AntShapeTypePtr shapeType;
	QString actualName = name;
	if ( name.isEmpty() == true ) {
		actualName = QString("body part %1").arg(d_experiment->AntShapeTypes().size()+1);
	}

	try {
		qDebug() << "[AntShapeTypeBridge]: Calling fmp::Experiment::CreateAntShapeType("
		         << actualName << ")";
		shapeType = d_experiment->CreateAntShapeType(ToStdString(actualName));
	} catch ( const std::exception & e ) {
		qCritical() << "Could not create AntShapeType " << name
		            << ": " << e.what();
		return;
	}
	d_model->appendRow(buildTypeItem(shapeType));
	setModified(true);
	emit typeModified(shapeType->TypeID(),ToQString(shapeType->Name()));
}

void AntShapeTypeBridge::deleteType(quint32 typeID) {
	if ( !d_experiment ) {
		return;
	}
	auto items = d_model->findItems(QString::number(typeID),Qt::MatchExactly,1);
	if ( items.isEmpty() == true ) {
		qWarning() << "Could not find type " << typeID;
		return;
	}
	try {
		qDebug() << "[AntShapeTypeBridge]: Calling fmp::Experiment::DeleteAntShapeType("
		         << typeID << ")";
		d_experiment->DeleteAntShapeType(typeID);
	} catch ( const std::exception & e ) {
		qCritical() << "Could not delete AntShapeType " << typeID
		            << ": " << e.what();
		return;
	}
	d_model->removeRows(items[0]->row(),1);
	setModified(true);
	emit typeDeleted(typeID);
}


void AntShapeTypeBridge::onTypeItemChanged(QStandardItem * item) {
	if ( item->column() != 0 ) {
		return;
	}

	auto shapeType = item->data().value<fmp::AntShapeType::Ptr>();
	if ( !shapeType ) {
		return;
	}
	try {
		shapeType->SetName(ToStdString(item->text()));
	} catch ( const std::exception & e ) {
		qCritical() << "Could not set ShapeTypeID " << shapeType->TypeID()
		            << " name to " << item->text() << ": " << e.what();
		item->setText(ToQString(shapeType->Name()));
		return;
	}
	setModified(true);
	emit typeModified(shapeType->TypeID(),item->text());
}


QList<QStandardItem*> AntShapeTypeBridge::buildTypeItem(const fmp::AntShapeType::Ptr & shapeType) {
	auto data = QVariant::fromValue(shapeType);
	auto id = new QStandardItem(QString::number(shapeType->TypeID()));
	id->setEditable(false);
	id->setData(data);
	auto icon = Conversion::iconFromFM(fmp::Palette::Default().At(shapeType->TypeID()));
	auto name = new QStandardItem(ToQString(shapeType->Name()));
	name->setEditable(true);
	name->setData(data);
	name->setIcon(icon);

	return {name,id};
}


AntShapeTypeBridge::AntShapeTypesByID AntShapeTypeBridge::types() const {
	AntShapeTypesByID res;
	for ( const auto & [ stID,type ] : d_experiment->AntShapeTypes() ) {
		res.insert(std::make_pair(stID,type));
	}
	return res;
}
