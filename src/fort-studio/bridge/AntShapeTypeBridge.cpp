#include "AntShapeTypeBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/AntShapeType.hpp>

#include <fort-studio/Format.hpp>

#include <fort-studio/widget/base/ColorComboBox.hpp>

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

void AntShapeTypeBridge::setExperiment(const fmp::ExperimentPtr & experiment) {
	setModified(false);
	d_model->clear();
	d_experiment = experiment;

	if ( !d_experiment ) {
		emit activated(false);
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
	try {
		qDebug() << "[AntShapeTypeBridge]: Calling fmp::Experiment::CreateAntShapeType("
		         << name << ")";
		shapeType = d_experiment->CreateAntShapeType(ToStdString(name));
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
	auto items = d_model->findItems(QString::number(typeID));
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
	if ( item->column() != 1 ) {
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
	QPixmap pixmap(20,20);
	pixmap.fill(ColorComboBox::fromMyrmidon(fmp::Palette::Default().At(shapeType->TypeID())));
	id->setIcon(pixmap);
	auto name = new QStandardItem(ToQString(shapeType->Name()));
	name->setEditable(true);
	id->setData(data);
	return {id,name};
}
