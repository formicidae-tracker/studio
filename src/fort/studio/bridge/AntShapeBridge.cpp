#include "AntShapeBridge.hpp"

#include <fort/myrmidon/priv/Identifier.hpp>

#include <QDebug>

#include "ExperimentBridge.hpp"
#include "AntShapeTypeBridge.hpp"
#include "AntGlobalModel.hpp"

#include <fort/studio/Format.hpp>


AntShapeBridge::AntShapeBridge(QObject * parent)
	: GlobalBridge(parent)
	, d_model(new AntGlobalModel(this)) {
	qRegisterMetaType<fmp::Capsule::ConstPtr>();
}

AntShapeBridge::~AntShapeBridge() {
}

QAbstractItemModel * AntShapeBridge::model() const {
	return d_model;
}

void AntShapeBridge::initialize(ExperimentBridge * experiment) {
	d_model->initialize(experiment->identifier());

	connect(experiment->antShapeTypes(),&AntShapeTypeBridge::typeModified,
	        this,&AntShapeBridge::onTypeModified);

	connect(experiment->antShapeTypes(),&AntShapeTypeBridge::typeDeleted,
	        this,&AntShapeBridge::onTypeDeleted);


	connect(experiment,&ExperimentBridge::antCreated,
	        this,&AntShapeBridge::onAntCreated);

	connect(experiment,&ExperimentBridge::antDeleted,
	        this,&AntShapeBridge::onAntDeleted);
}

int AntShapeBridge::addCapsule(fm::Ant::ID antID,
                               fmp::AntShapeTypeID typeID,
                               const fmp::CapsulePtr & capsule) {
	auto ant = AntGlobalModel::findAnt(d_experiment,antID);
	auto antItem = d_model->itemFromAntID(antID);
	int column = -1;
	try {
		column = d_columnIndex.at(typeID);
	} catch ( const std::exception & ) {
	}
	if (ant == nullptr || antItem == nullptr || column == -1) {
		return -1;
	}
	auto countItem = d_model->item(antItem->row(),column);
	try {
		qDebug() << "[AntShapeBridge]: Calling fmp::Ant::AddCapsule("
		         << ant->FormattedID().c_str()
		         << "," << typeID
		         << "," << ToQString(*capsule)
		         << ")";

		ant->AddCapsule(typeID,*capsule);
	} catch ( const std::exception & e) {
		qCritical() << "[AntShapeBridge]: Could not add capsule to Ant "
		            << ant->FormattedID().c_str()
		            << ": " << e.what();
		return -1;
	}

	qInfo() << "[AntShapeBridge]: added capsule "
	        << ToQString(*capsule)
	        << " to ant "
	        << ant->FormattedID().c_str();

	countItem->setData(countItem->data(Qt::DisplayRole).toInt() +  1,
	                   Qt::DisplayRole);

	setModified(true);
	emit capsuleCreated(ant->AntID(),
	                    ant->Capsules().size(),
	                    typeID,
	                    capsule);

	return ant->Capsules().size();
}

void AntShapeBridge::clearCapsule(fm::Ant::ID antID) {
	auto ant = AntGlobalModel::findAnt(d_experiment,antID);
	auto antItem = d_model->itemFromAntID(antID);
	if ( ant == nullptr || antItem == nullptr ) {
		return;
	}

	try {
		qDebug() << "[AntShapeBridge]: Calling fmp::Ant("
		         << ant->FormattedID().c_str()
		         << ")::DeleteCapsules()";
		ant->ClearCapsules();
	} catch ( const std::exception & e ) {
		qCritical() << "[AntShapeBridge]: Could not remove Capsules "
		            << " from Ant " << ant->FormattedID().c_str()
		            << ": " << e.what();
		return;
	}
	qInfo() << "[AntShapeBridge]: cleared capsule for ant "
	        << ant->FormattedID().c_str();

	for ( size_t i = 1; i < d_model->columnCount(); ++i ) {
		auto item = d_model->item(antItem->row(),i);
		item->setData(0,Qt::DisplayRole);
	}

	setModified(true);
	emit capsuleCleared(ant->AntID());
}

void AntShapeBridge::cloneShape(fm::Ant::ID source,
                              bool scaleToSize,
                              bool overwriteShape) {
	auto ant = AntGlobalModel::findAnt(d_experiment,source);
	if (ant == nullptr ) {
		return;
	}
	std::set<fm::AntID> touched;
	for ( const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		if ( overwriteShape == true
		     || ant->Capsules().empty() == true ) {

			touched.insert(antID);
		}
	}
	touched.erase(source);

	try {
		qDebug() << "[AntShapeBridge]: Calling fmp::Experiment::CloneAntShape("
		         << ant->FormattedID().c_str() << ","
		         << scaleToSize << ","
		         << overwriteShape << ")";
		d_experiment->CloneAntShape(source,
		                            scaleToSize,
		                            overwriteShape);
	} catch ( const std::exception & e ) {
		qCritical() << "[AntShapeBridge]: Could not clone Ant shape: "
		            << e.what();
		return;
	}

	for ( const auto & antID : touched ) {
		if ( overwriteShape == true ) {
			emit capsuleCleared(antID);
		}
		countAnt(antID,true);
	}
	setModified(touched.empty() == false );
}

void AntShapeBridge::tearDownExperiment() {
	d_model->clear();
	d_model->setHorizontalHeaderLabels({tr("Ant")});
	d_columnIndex.clear();
}

void AntShapeBridge::setUpExperiment() {
	if ( isActive() == false ) {
		return;
	}

	for ( const auto & [typeID,type] : d_experiment->CAntShapeTypes() ) {
		onTypeModified(typeID,type->Name().c_str());
	}

	for ( const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		d_model->insertRow(d_model->rowCount(),buildAnt(ant));
	}

	for ( const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		countAnt(antID,false);
	}
}

void AntShapeBridge::onTypeModified(quint32 shapeTypeID,
                                    QString name) {
	auto fi = d_columnIndex.lower_bound(shapeTypeID);
	if ( fi != d_columnIndex.cend() && fi->first == shapeTypeID ) {
		d_model->horizontalHeaderItem(fi->second)->setData(name,Qt::DisplayRole);
		return;
	}
	int column = d_model->columnCount();
	if ( fi != d_columnIndex.cend() ) {
		column = fi->second;
	}
	QList<QStandardItem*> newCounts;
	for ( size_t i = 0; i < d_model->rowCount(); ++i ) {
		auto count = new QStandardItem("0");
		count->setEditable(false);
		newCounts.push_back(count);
	}
	d_model->insertColumn(column,newCounts);
	auto item = new QStandardItem(name);
	item->setEditable(false);
	item->setData(shapeTypeID);
	d_model->setHorizontalHeaderItem(column,item);
	rebuildColumnIndex();
}

void AntShapeBridge::onTypeDeleted(quint32 shapeTypeID) {
	auto fi = d_columnIndex.find(shapeTypeID);
	if ( fi == d_columnIndex.end() ) {
		return;
	}
	d_model->removeColumns(fi->second,1);
	rebuildColumnIndex();
}

void AntShapeBridge::onAntCreated(quint32 antID) {
	auto ant = AntGlobalModel::findAnt(d_experiment,antID);
	if ( ant == nullptr ) {
		return;
	}
	d_model->insertRow(d_model->rowCount(),buildAnt(ant));
}

void AntShapeBridge::onAntDeleted(quint32 antID) {
	auto antItem = d_model->itemFromAntID(antID);
	if ( antItem == nullptr ) {
		return;
	}
	d_model->removeRows(antItem->row(),1);
}

QList<QStandardItem*> AntShapeBridge::buildAnt(const fmp::Ant::Ptr & ant) {
	auto antItem = new QStandardItem(AntGlobalModel::formatAntName(ant));
	AntGlobalModel::setItemUserData(antItem,ant);
	QList<QStandardItem*> res = {antItem};
	for ( const auto & t : d_experiment->CAntShapeTypes() ) {
		auto countItem = new QStandardItem("0");
		res.push_back(countItem);
	}

	for ( const auto & item : res ) {
		item->setEditable(false);
	}
	return res;
}


void AntShapeBridge::countAnt(fm::Ant::ID antID,
                              bool sendSignals) {
	auto ant = AntGlobalModel::findAnt(d_experiment,antID);
	auto antItem = d_model->itemFromAntID(antID);
	if ( ant == nullptr ) {
		return;
	}
	std::map<fmp::AntShapeTypeID,size_t> counts;

	for ( const auto & [typeID,shapeType] : d_experiment->CAntShapeTypes() ) {
		counts[typeID] = 0;
	}
	int index = -1;
	for ( const auto [typeID,capsule] : ant->Capsules() ) {
		++index;
		counts[typeID] += 1;
		if ( sendSignals == true ) {
			emit capsuleCreated(antID,index,typeID,std::make_shared<fmp::Capsule>(capsule));
		}
	}

	for ( const auto & [typeID,count] : counts ) {
		d_model->item(antItem->row(),d_columnIndex[typeID])->setData(int(count),Qt::DisplayRole);
	}

}

void AntShapeBridge::rebuildColumnIndex() {
	d_columnIndex.clear();

	if ( isActive() == false ) {
		return;
	}
	int index = 0;
	for ( const auto & [typeID,type] : d_experiment->CAntShapeTypes() ) {
		d_columnIndex[typeID] = ++index;
	}
}
