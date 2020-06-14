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
	, d_timedChangeModel(new QStandardItemModel(this) )
	, d_selectedAntID(0) {
	qRegisterMetaType<fmp::AntMetadata::Column::Ptr>();
	qRegisterMetaType<fmp::Ant::Ptr>();
	qRegisterMetaType<fm::Time::ConstPtr>();

	connect(d_columnModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &AntMetadataBridge::onColumnItemChanged);

	connect(d_dataModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &AntMetadataBridge::onDataItemChanged);

	connect(d_timedChangeModel,
	        &QStandardItemModel::itemChanged,
	        this,
	        &AntMetadataBridge::onTimedChangeItemChanged);


#define add_item_type(t,T,tT) do {	  \
		auto  item = new QStandardItem(#tT); \
		item->setData(quint32(fmp::AntMetadata::Type::T)); \
		d_typeModel->appendRow(item); \
	}while(0)
	add_item_type(bool,BOOL,Bool);
	add_item_type(int,INT,Int);
	add_item_type(double,DOUBLE,Double);
#undef add_item_type

	auto stringItem = new QStandardItem("String");
	stringItem->setData(quint32(fmp::AntMetadata::Type::STRING));
	d_typeModel->appendRow(stringItem);

	auto timeItem = new QStandardItem("Time");
	timeItem->setData(quint32(fmp::AntMetadata::Type::TIME));
	d_typeModel->appendRow(timeItem);

	connect(this,
	        &AntMetadataBridge::metadataColumnChanged,
	        this,
	        &AntMetadataBridge::rebuildDataModel);

	connect(this,
	        &AntMetadataBridge::metadataColumnRemoved,
	        this,
	        &AntMetadataBridge::rebuildDataModel);

	selectRow(-1);
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


	for ( const auto & [name,column] : d_experiment->AntMetadataPtr()->Columns() ) {
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

	auto valueItem = new QStandardItem(ToQString(column->DefaultValue()));
	valueItem->setEditable(true);
	valueItem->setData(data);

	return {nameItem,typeItem,valueItem};
}


void AntMetadataBridge::onColumnItemChanged(QStandardItem * item) {
	auto column = item->data(Qt::UserRole+1).value<fmp::AntMetadata::Column::Ptr>();
	if ( !column ) {
		return;
	}

	if ( item->column() == 2 ) {

		try {
			auto v = fmp::AntMetadata::FromString(column->MetadataType(),ToStdString(item->text()));
			qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::AntMetadata::Column::SetDefaultValue(" << ToQString(v) << ")";
			column->SetDefaultValue(v);
			QSignalBlocker blocker(d_columnModel);
			item->setText(ToQString(column->DefaultValue()));
		} catch ( const std::exception & e) {
			qCritical() << "Could not set default value for column '" << ToQString(column->Name())
			            << "' to '" << item->text() << "': " << e.what();
			QSignalBlocker blocker(d_columnModel);
			item->setText(ToQString(column->DefaultValue()));
			return;
		}

		qInfo() << "Set default value for column '" << ToQString(column->Name())
		        << "' to " << item->text();

		setModified(true);
		emit metadataColumnChanged(ToQString(column->Name()),quint32(column->MetadataType()));
		return;
	}

	if ( item->column() == 1 ) {
		auto type = item->data(Qt::UserRole+2).toInt();
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

		{
			QSignalBlocker blocker(d_columnModel);
			auto valueItem = d_columnModel->item(item->row(),2);
			valueItem->setText(ToQString(column->DefaultValue()));
		}


		setModified(true);
		emit metadataColumnChanged(ToQString(column->Name()),type);
		return;
	}

	if ( item->column() == 0 ) {
		if ( column->Name() == ToStdString(item->text()) ) {
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
			QSignalBlocker blocker(d_columnModel);
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


void AntMetadataBridge::onAntListModified() {
	rebuildDataModel();


}

void AntMetadataBridge::rebuildDataModel() {
	d_dataModel->clear();
	QStringList labels = { tr("Ant"), tr("Timed Changes") };
	selectRow(-1);
	if ( !d_experiment  ) {
		return;
	}
	const auto & columns = d_experiment->AntMetadataPtr()->Columns();
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
		size_t count = 0;
		for ( const auto & [name,tValues] : ant->DataMap() ) {
			for ( const auto & value : tValues ) {
				if ( !value.first ) { continue; }
				++count;
			}
		}
		auto timedChangeCount = new QStandardItem(QString::number(count));
		timedChangeCount->setData(antData);
		timedChangeCount->setEditable(false);
		QList<QStandardItem*> line = {antLabel,timedChangeCount};
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
	fm::AntStaticValue v;
	try {
		v = ant->GetBaseValue(column->Name());
		isDefault = false;
	} catch ( const std::exception & e ) {
		v = column->DefaultValue();
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
	if ( item->text().isEmpty() == true ) {
		try {
			qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::Ant::DeleteValue('"
			         << ToQString(col->Name()) << "',0)";
			ant->DeleteValue(col->Name(),fm::Time::ConstPtr());
			setModified(true);
		} catch ( const std::exception & e ) {
			qWarning() << "Could not set " << ant->FormattedID().c_str()
			           << " column '" << ToQString(col->Name())
			           << "' to default value: " << e.what();
		}
		qInfo() << "Deleted base value for ant " << ant->FormattedID().c_str();
		QSignalBlocker blocker(d_dataModel);
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
		qCritical() << "Could not set " << ant->FormattedID().c_str()
		            << " column '" << ToQString(col->Name())
		            << "' to '" << item->text()
		            << "': " << e.what();
	}
	QSignalBlocker blocker(d_dataModel);
	setupItemFromValue(item,ant,col);
	qInfo() << "Set base value for Ant " << ant->FormattedID().c_str()
	        << " '" << ToQString(col->Name()) << "' to " << item->text();
}


void AntMetadataBridge::selectRow(int row) {
	d_timedChangeModel->clear();
	d_timedChangeModel->setHorizontalHeaderLabels({tr("Column Name"),tr("Date"),tr("Value")});

	if ( !d_experiment || row < 0 || row > d_dataModel->rowCount() ) {
		setSelectedAntID(0);
		return;
	}

	auto ant = d_dataModel->index(row,0).data(Qt::UserRole+1).value<fmp::Ant::Ptr>();
	if (!ant) {
		setSelectedAntID(0);
		return;
	}

	setSelectedAntID(ant->AntID());

	for ( const auto & [name,tValues] : ant->DataMap() ) {
		fmp::AntMetadata::Column::Ptr column;
		try {
			column = d_experiment->AntMetadataPtr()->Columns().at(name);
		} catch ( const std::exception & e) {
			qWarning() << "Could not find column '" << ToQString(name);
			selectRow(-1);
		}
		for ( const auto & value : tValues ) {
			if ( !value.first ) {
				continue;
			}
			d_timedChangeModel->appendRow(buildTimedChange(ant,
			                                               column,
			                                               *value.first,
			                                               value.second));
		}
	}
	d_timedChangeModel->sort(0,Qt::AscendingOrder);
	d_timedChangeModel->sort(1,Qt::AscendingOrder);
}

quint32 AntMetadataBridge::selectedAntID() const {
	return d_selectedAntID;
}

void AntMetadataBridge::setSelectedAntID(quint32 ID) {
	if ( d_selectedAntID == ID ) {
		return;
	}
	d_selectedAntID = ID;
	emit selectedAntIDChanged(d_selectedAntID);
}

QList<QStandardItem*> AntMetadataBridge::buildTimedChange(const fmp::Ant::Ptr & ant,
                                                          const fmp::AntMetadata::Column::Ptr & column,
                                                          const fm::Time & time,
                                                          const fm::AntStaticValue & value) {
	auto data = QVariant::fromValue(ant);
	auto columnData = QVariant::fromValue(column);
	auto name = ToQString(column->Name());
	auto nameItem = new QStandardItem(name);
	nameItem->setData(data);
	nameItem->setData(columnData,Qt::UserRole+2);
	nameItem->setData(name,Qt::UserRole+3);
	nameItem->setEditable(false);
	auto timeItem = new QStandardItem(ToQString(time));
	timeItem->setData(data);
	timeItem->setData(columnData,Qt::UserRole+2);
	timeItem->setData(ToQString(time),Qt::UserRole+3);
	auto valueItem = new QStandardItem(ToQString(value));
	valueItem->setData(data);
	valueItem->setData(columnData,Qt::UserRole+2);
	valueItem->setData(ToQString(value),Qt::UserRole+3);

	return {nameItem,timeItem,valueItem};
}

void AntMetadataBridge::addTimedChange(quint32 antID,const QString & name) {
	if ( !d_experiment ) {
		return;
	}
	auto antLabel = ToQString(fmp::Ant::FormatID(antID));
	auto items = d_dataModel->findItems(antLabel);
	if ( items.isEmpty() == true ) {
		qWarning() << "Could not found ant " << antLabel;
		return;
	}
	auto antItem = items[0];
	auto ant = antItem->data(Qt::UserRole+1).value<fmp::Ant::Ptr>();

	items = d_columnModel->findItems(name);
	if ( items.isEmpty() == true ) {
		qWarning() << "Could not found column " << name;
		return;
	}


	auto column = items[0]->data(Qt::UserRole+1).value<fmp::AntMetadata::Column::Ptr>();
	if ( !ant || !column ) {
		return;
	}

	try {
		qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::Ant::SetValue("
		         << name << "," << ToQString(column->DefaultValue()) << ","
		         << ToQString(fm::Time()) << ")";


		ant->SetValue(column->Name(),column->DefaultValue(),std::make_shared<fm::Time>(),true);
	} catch ( const std::exception & e ) {
		qCritical() << "Could not set Ant " << antLabel << " column " << name << " at "
		            << ToQString(fm::Time()) << " to " << ToQString(column->DefaultValue())
		            << ": " << e.what();
		return;
	}
	qInfo() << "Set Ant " << antLabel << " column " << name << " at "
	        << ToQString(fm::Time()) << " to "
	        << ToQString(column->DefaultValue());

	setModified(true);

	auto countItem = d_dataModel->itemFromIndex(d_dataModel->index(antItem->row(),1));
	countItem->setText(QString::number(countItem->text().toInt()+1));

	d_timedChangeModel->appendRow(buildTimedChange(ant,column,fm::Time(),column->DefaultValue()));

	d_timedChangeModel->sort(0,Qt::AscendingOrder);
	d_timedChangeModel->sort(1,Qt::AscendingOrder);

}


void AntMetadataBridge::removeTimedChange(const QModelIndex & index) {
	auto ant = index.data(Qt::UserRole+1).value<fmp::Ant::Ptr>();
	if ( !ant || index.row() >= d_timedChangeModel->rowCount() ) {
		return;
	}
	auto antLabel = ToQString(ant->FormattedID());
	auto name = d_timedChangeModel->index(index.row(),0).data(Qt::DisplayRole).toString();
	auto timeStr = d_timedChangeModel->index(index.row(),1).data(Qt::DisplayRole).toString();
	fm::Time::ConstPtr time;
	try {
		time = std::make_shared<fm::Time>(fm::Time::Parse(ToStdString(timeStr)));
	} catch ( const std::exception & e ) {
		qWarning() << "Could not parse '" << timeStr << "': " << e.what();
		return;
	}

	try {
		qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::priv::Ant::DeleteValue("
		         << name << "," << timeStr << ")";
		ant->DeleteValue(ToStdString(name),time);
	} catch (const std::exception & e) {
		qCritical() << "Could not delete Ant " << antLabel
		            << " value " << name << " at " << timeStr
		            << ": " << e.what();
		return;
	}
	qInfo() << "Removed Ant " << antLabel
	        << " value " << name << " at "
	        << timeStr;
	setModified(true);

	auto items = d_dataModel->findItems(antLabel);
	if ( items.isEmpty() == true ) {
		qWarning() << "Could not update count";
	} else {
		auto countItem = d_dataModel->itemFromIndex(d_dataModel->index(items[0]->row(),1));
		countItem->setText(QString::number(countItem->text().toInt()-1));
	}

	d_timedChangeModel->removeRows(index.row(),1);
}


void AntMetadataBridge::onTimedChangeItemChanged(QStandardItem * item) {
	auto ant = item->data().value<fmp::Ant::Ptr>();
	auto column = item->data(Qt::UserRole+2).value<fmp::AntMetadata::Column::Ptr>();

	if (!ant || !column ||  !d_experiment ) {
		item->setText(item->data(Qt::UserRole+3).toString());
		return;
	}
	if ( item->text() == item->data(Qt::UserRole+3).toString() ) {
		return;
	}

	QStandardItem *nameItem,*timeItem,*valueItem;
	std::string name;
	fm::Time::ConstPtr time;
	fm::AntStaticValue value;
	try {
		nameItem = d_timedChangeModel->itemFromIndex(d_timedChangeModel->index(item->row(),0));
		name = ToStdString(nameItem->data(Qt::UserRole+3).toString());
		timeItem = d_timedChangeModel->itemFromIndex(d_timedChangeModel->index(item->row(),1));
		time = std::make_shared<fm::Time>(fm::Time::Parse(ToStdString(timeItem->data(Qt::UserRole+3).toString())));
		valueItem = d_timedChangeModel->itemFromIndex(d_timedChangeModel->index(item->row(),2));
		value = fmp::AntMetadata::FromString(column->MetadataType(),ToStdString(valueItem->data(Qt::UserRole+3).toString()));
	} catch ( const std::exception & e) {
		qWarning() << "Could not retrieve time change data: " << e.what();
		item->setText(item->data(Qt::UserRole+3).toString());
		return;
	}
	if ( item->column() == 1 ) {

		fmp::Time::ConstPtr newTime;
		try {
			newTime = std::make_shared<fm::Time>(fm::Time::Parse(ToStdString(item->text())));
		} catch (const std::exception & e) {
			qWarning() << "Could not parse time " << item->text();
			item->setText(item->data(Qt::UserRole+3).toString());
			return;
		}

		try {
			qDebug() << "[AntMetadataBridge]: Changing column name '"
			         << nameItem->text() << "' time from "
			         << item->data(Qt::UserRole+3).toString()
			         << " to " << item->text();
			ant->DeleteValue(name,time);
			ant->SetValue(name,value,newTime,true);
			qInfo() << "Changed timed change " << nameItem->text()
			        << " from " << item->data(Qt::UserRole+3).toString()
			        << " to " << item->text();
			setModified(true);
			QSignalBlocker blocker(d_timedChangeModel);
			item->setText(ToQString(*newTime));
			item->setData(item->text(),Qt::UserRole+3);
		} catch ( const std::exception & e) {
			qCritical() << "Could not change column "
			            << nameItem->text() << " time from "
			            << item->data(Qt::UserRole+3).toString()
			            << " to " << item->text()
			            << ": " << e.what();
			item->setText(item->data(Qt::UserRole+3).toString());
		}
	}

	if ( item->column() == 2 ) {
		fm::AntStaticValue newValue;
		try {
			newValue = fmp::AntMetadata::FromString(column->MetadataType(),ToStdString(item->text()));
		} catch ( const std::exception & e ) {
			qWarning() << "Could not parse AntStaticValue " << item->text()
			           << ": " << e.what();
			item->setText(item->data(Qt::UserRole+3).toString());
			return;
		}

		try {
			qDebug() << "[AntMetadataBridge]: Calling fort::myrmidon::Ant::SetValue('"
			         << nameItem->text() << "',"
			         << item->text() << ","
			         << timeItem->text()
			         << ")";
			ant->SetValue(name,newValue,time);
			qInfo() << "Changed timed change " << nameItem->text()
			        << " at "
			        << timeItem->text()
			        << " from " << item->data(Qt::UserRole+3).toString()
			        << " to " << item->text();
			setModified(true);
			QSignalBlocker blocker(d_timedChangeModel);
			item->setText(ToQString(newValue));
			item->setData(item->text(),Qt::UserRole+3);
		} catch ( const std::exception & e ) {
			qCritical() << "Could not change timed change '"
			            << nameItem->text()
			            << " at " << timeItem->text()
			            << " from " << item->data(Qt::UserRole+3).toString()
			            << " to " << item->text()
			            << ": " << e.what();
			item->setText(item->data(Qt::UserRole+3).toString());
		}
	}

}
