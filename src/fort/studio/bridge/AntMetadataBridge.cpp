#include "AntMetadataBridge.hpp"

#include <QStandardItemModel>
#include <QDebug>
#include <QSignalBlocker>

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>

#include <fort/studio/Format.hpp>

AntMetadataBridge::AntMetadataBridge(QObject * parent)
	: Bridge(parent)
	, d_columnModel(new QStandardItemModel(this))
	, d_typeModel(new QStandardItemModel(this))
	, d_dataModel(new QStandardItemModel(this))
	, d_timedChangeModel(new QStandardItemModel(this) ) {
	qRegisterMetaType<fmp::AntMetadata::Column::Ptr>();

	connect(d_columnModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &AntMetadataBridge::onColumnItemChanged);

	connect(d_dataModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &AntMetadataBridge::onDataItemChanged);


#define add_item_type(t,T) do {	  \
		auto  item = new QStandardItem(#T); \
		item->setData(quint32(fmp::AntMetadata::Type::T)); \
		item->setData(std::get<t>(fmp::AntMetadata::DefaultValue(fmp::AntMetadata::Type::T)),Qt::UserRole+2); \
		d_typeModel->appendRow(item); \
	}while(0)
	add_item_type(bool,Bool);
	add_item_type(int,Int);
	add_item_type(double,Double);
#undef add_item_type

	auto stringItem = new QStandardItem("String");
	stringItem->setData(quint32(fmp::AntMetadata::Type::String));
	stringItem->setData(ToQString(std::get<std::string>(fmp::AntMetadata::DefaultValue(fmp::AntMetadata::Type::String))),Qt::UserRole+2);
	d_typeModel->appendRow(stringItem);

	auto timeItem = new QStandardItem("Time");
	timeItem->setData(quint32(fmp::AntMetadata::Type::Time));
	timeItem->setData(ToQString(std::get<fm::Time>(fmp::AntMetadata::DefaultValue(fmp::AntMetadata::Type::Time))),Qt::UserRole+2);
	d_typeModel->appendRow(timeItem);

	connect(this,
	        &AntMetadataBridge::metadataColumnChanged,
	        this,
	        &AntMetadataBridge::rebuildDataModel);

	connect(this,
	        &AntMetadataBridge::metadataColumnRemoved,
	        this,
	        &AntMetadataBridge::rebuildDataModel);


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


	for ( const auto & [name,column] : d_experiment->AntMetadataConstPtr()->Columns() ) {
		d_columnModel->appendRow(buildColumn(column));
	}

	rebuildDataModel();

	emit activated(true);
}

QAbstractItemModel * AntMetadataBridge::columnModel() {
	return d_columnModel;
}

QAbstractItemModel * AntMetadataBridge::dataModel() {
	return d_dataModel;
}

QAbstractItemModel * AntMetadataBridge::typeModel() {
	return d_typeModel;
}

QAbstractItemModel * AntMetadataBridge::timedChangeModel() {
	return d_timedChangeModel;
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

	auto typeItem = new QStandardItem(findTypeName(column->MetadataType()));
	typeItem->setData(data);
	typeItem->setData(quint32(column->MetadataType()),Qt::UserRole+2);

	auto valueItem = new QStandardItem(findTypeDefaultValue(column->MetadataType()).toString());
	valueItem->setEditable(false);
	valueItem->setData(data);

	return {nameItem,typeItem,valueItem};
}


void AntMetadataBridge::onColumnItemChanged(QStandardItem * item) {

	if ( item->column() == 1 ) {
		auto type = item->data(Qt::UserRole+2).toInt();
		auto column = item->data(Qt::UserRole+1).value<fmp::AntMetadata::Column::Ptr>();
		if (!column) {
			return;
		}
		if ( type == int(column->MetadataType()) ) {
			return;
		}
		try {
			qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::AntMetadata::Column::SetMetadataType(" << type << ")";
			column->SetMetadataType(fmp::AntMetadata::Type(type));
		} catch (const std::exception & e) {
			qCritical() << "Could not change type for column '" << ToQString(column->Name())
			            << "': " << e.what();
			return;
		}
		auto newTypeStr = findTypeName(column->MetadataType());

		qInfo() << "Changed AntMetadataColumn '" << ToQString(column->Name())
		        << "' type from " << item->text() << " to " << newTypeStr;
		item->setText(newTypeStr);


		auto valueItem = d_columnModel->item(item->row(),2);
		valueItem->setText(findTypeDefaultValue(column->MetadataType()).toString());

		setModified(true);
		emit metadataColumnChanged(ToQString(column->Name()),type);
		return;
	}

	if ( item->column() == 0 ) {
		auto column = item->data(Qt::UserRole+1).value<fmp::AntMetadata::Column::Ptr>();
		if ( !column || column->Name() == ToStdString(item->text()) ) {
			return;
		}
		auto oldName = ToQString(column->Name());
		try {
			qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::AntMetadata::Column::SetName("
			         << item->text() << ")";
			column->SetName(ToStdString(item->text()));
		} catch (const std::exception & e) {
			qCritical() << "Could not change name for column '" << ToQString(column->Name())
			            << "' to '" << item->text()
			            << "': " << e.what();
			item->setText(ToQString(column->Name()));
			return;
		}
		qInfo() << "Changed AntMetadataColumn name from '"
		        << oldName << "' to '" << item->text()
		        << "'" ;
		setModified(true);
		d_columnModel->sort(0,Qt::AscendingOrder);
		emit metadataColumnChanged(ToQString(column->Name()),quint32(column->MetadataType()));
		return;
	}

}


QString AntMetadataBridge::findTypeName(fmp::AntMetadata::Type type) {
	for ( int i = 0; i < d_typeModel->rowCount(); ++i ) {
		auto item = d_typeModel->item(i,0);
		if ( item == nullptr ) {
			continue;
		}
		if ( item->data().toInt() == quint32(type) ) {
			return item->text();
		}
	}
	return "<unknown>";
}

QVariant AntMetadataBridge::findTypeDefaultValue(fmp::AntMetadata::Type type) {
	for ( int i = 0; i < d_typeModel->rowCount(); ++i ) {
		auto item = d_typeModel->item(i,0);
		if ( item == nullptr ) {
			continue;
		}
		if ( item->data().toInt() == quint32(type) ) {
			return item->data(Qt::UserRole+2);
		}
	}
	return "<unknown>";
}

void AntMetadataBridge::onAntListModified() {
	rebuildDataModel();


}

void AntMetadataBridge::rebuildDataModel() {
	d_dataModel->clear();
	QStringList labels = { tr("Ant") };
	if ( !d_experiment  ) {
		return;
	}
	const auto & columns = d_experiment->AntMetadataConstPtr()->Columns();
	const auto & ants  = d_experiment->Identifier()->Ants();
	for ( const auto & [name,column] : columns ) {
		labels.push_back(ToQString(name));
	}
	d_dataModel->setHorizontalHeaderLabels(labels);


	for ( const auto & [aID,ant] : ants ) {
		auto antData = QVariant::fromValue(ant);

		auto antLabel = new QStandardItem(ToQString(fmp::Ant::FormatID(aID)));
		antLabel->setEditable(false);
		antLabel->setData(antData);
		QList<QStandardItem*> line = {antLabel};
		for ( const auto & [name,column] : columns ) {
			auto colData = QVariant::fromValue(column);
			auto item = new QStandardItem("");
			item->setData(antData);
			item->setData(colData,Qt::UserRole+2);

			setupItemFromValue(item,ant,column);
			line.push_back(item);
		}
		d_dataModel->appendRow(line);

	}
	d_dataModel->sort(0,Qt::AscendingOrder);
}



void AntMetadataBridge::setupItemFromValue(QStandardItem * item,
                                           const fmp::Ant::ConstPtr & ant,
                                           const fmp::AntMetadata::Column::ConstPtr & column) {
	bool isDefault = true;
	fmp::AntStaticValue v;
	try {
		v = ant->GetBaseValue(column->Name());
		isDefault = false;
	} catch ( const std::exception & e ) {
		v = fmp::AntMetadata::DefaultValue(column->MetadataType());
	}
	item->setText(std::visit([](auto && args){ return ToQString(args);},v));
	if ( isDefault == true ) {
		item->setData(QColor(0,0,255),Qt::ForegroundRole);
	} else {
		item->setData(QVariant(),Qt::ForegroundRole);
	}
}


void AntMetadataBridge::onDataItemChanged(QStandardItem * item) {
	auto ant = item->data(Qt::UserRole+1).value<fmp::Ant::Ptr>();
	auto col = item->data(Qt::UserRole+2).value<fmp::AntMetadata::Column::Ptr>();

	if ( !ant == true || !col == true ) {
		return;
	}
	QSignalBlocker blocker(this);
	if ( item->text().isEmpty() == true ) {
		try {
			qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::Ant::DeleteValue('"
			         << ToQString(col->Name()) << "',0)";
			ant->DeleteValue(col->Name(),fm::Time::ConstPtr());
			setModified(true);
		} catch ( const std::exception & e ) {
			qWarning() << "Could not set " << ToQString(fmp::Ant::FormatID(ant->ID()))
			           << " column '" << ToQString(col->Name())
			           << "' to default value: " << e.what();
		}
		qInfo() << "Deleted base value for ant " << ToQString(fmp::Ant::FormatID(ant->ID()));
		setupItemFromValue(item,ant,col);
		return;
	}

	try {

		auto v = fmp::AntMetadata::FromString(col->MetadataType(),ToStdString(item->text()));
		try {
			auto actual = ant->GetBaseValue(col->Name());
			if ( v == actual ) {
				return;
			}
		} catch ( const std::exception & e) {
		}
		qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::Ant::SetValue('"
		         << ToQString(col->Name()) << "',"
		         << item->text() << ",0)";
		ant->SetValue(col->Name(),v,fm::Time::ConstPtr());

		setModified(true);
	} catch (const std::exception & e) {
		qCritical() << "Could not set " << ToQString(fmp::Ant::FormatID(ant->ID()))
		            << " column '" << ToQString(col->Name())
		            << "' to '" << item->text()
		            << "': " << e.what();
	}
	setupItemFromValue(item,ant,col);
	qInfo() << "Set base value for Ant " << ToQString(fmp::Ant::FormatID(ant->ID()))
	        << " '" << ToQString(col->Name()) << "' to " << item->text();
}


void AntMetadataBridge::selectRow(int row) {
}
