#include "AntMeasurementBridge.hpp"

#include "AntGlobalModel.hpp"

#include "ExperimentBridge.hpp"
#include "IdentifierBridge.hpp"
#include "MeasurementBridge.hpp"

#include <fort/myrmidon/priv/Identifier.hpp>

#include <fort/studio/MyrmidonTypes/Experiment.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>

AntMeasurementBridge::AntMeasurementBridge(QObject * parent)
	: GlobalBridge(parent)
	, d_model(new AntGlobalModel(this)) {
}

AntMeasurementBridge::~AntMeasurementBridge() {

}

void AntMeasurementBridge::initialize(ExperimentBridge * experiment) {
	d_model->initialize(experiment->identifier());

	connect(experiment->measurements(),&MeasurementBridge::measurementTypeModified,
	        this,&AntMeasurementBridge::onMeasurementTypeModified);

	connect(experiment->measurements(),&MeasurementBridge::measurementTypeDeleted,
	        this,&AntMeasurementBridge::onMeasurementTypeDeleted);

	connect(experiment->measurements(),&MeasurementBridge::measurementCreated,
	        this,&AntMeasurementBridge::onMeasurementCreated);

	connect(experiment->measurements(),&MeasurementBridge::measurementDeleted,
	        this,&AntMeasurementBridge::onMeasurementDeleted);


	connect(experiment,&ExperimentBridge::antCreated,
	        this,&AntMeasurementBridge::onAntCreated);

	connect(experiment,&ExperimentBridge::antDeleted,
	        this,&AntMeasurementBridge::onAntDeleted);

	connect(experiment->identifier(),&IdentifierBridge::identificationCreated,
	        this,&AntMeasurementBridge::onIdentificationModified);

	connect(experiment->identifier(),&IdentifierBridge::identificationRangeModified,
	        this,&AntMeasurementBridge::onIdentificationModified);

	connect(experiment->identifier(),&IdentifierBridge::identificationDeleted,
	        this,&AntMeasurementBridge::onIdentificationModified);

}

QAbstractItemModel * AntMeasurementBridge::model() const {
	return d_model;
}

void AntMeasurementBridge::tearDownExperiment() {
	d_model->clear();
	d_model->setHorizontalHeaderLabels({"Ant"});
}


void AntMeasurementBridge::setUpExperiment() {
	if ( isActive() == false ) {
		return;
	}

	for ( const auto & [mtID,mType] : d_experiment->MeasurementTypes() ) {
		onMeasurementTypeModified(mtID,mType->Name().c_str());
	}


	for (const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		d_model->insertRow(d_model->rowCount(),buildAnt(ant));
	}

	for (const auto & [antID,ant] : d_experiment->Identifier()->Ants() ) {
		buildCountForAnt(ant);
	}
}


void AntMeasurementBridge::onMeasurementTypeModified(quint32 mtID, QString name) {
	const auto & [headerItem,column] = headerForType(mtID);
	if ( headerItem == nullptr ) {
		QList<QStandardItem*> newCounts;
		newCounts.reserve(d_model->rowCount());
		for ( size_t i  = 0; i < d_model->rowCount(); ++i ) {
			auto count = new QStandardItem("0");
			count->setEditable(false);
			newCounts.push_back(count);
		}
		d_model->insertColumn(column,newCounts);
		auto item = new QStandardItem(name);
		item->setEditable(false);
		item->setData(int(mtID));
		d_model->setHorizontalHeaderItem(column,item);
	} else {
		headerItem->setData(name,Qt::DisplayRole);
	}
}

void AntMeasurementBridge::onMeasurementTypeDeleted(quint32 mtID) {
	auto [headerItem,column] = headerForType(mtID);
	if ( headerItem == nullptr ) {
		return;
	}
	d_model->removeColumns(column,1);
}

void AntMeasurementBridge::onAntCreated(quint32 antID) {
	try {
		auto ant = d_experiment->Identifier()->Ants().at(antID);
		d_model->insertRow(d_model->rowCount(),buildAnt(ant));
	} catch ( const std::exception &) {
	}
}

void AntMeasurementBridge::onAntDeleted(quint32 antID) {
	auto item = d_model->itemFromAntID(antID);
	if ( item == nullptr ) {
		return;
	}
	d_model->removeRows(item->row(),1);
}

void AntMeasurementBridge::onMeasurementCreated(const fmp::Measurement::ConstPtr & m) {
	updateMeasurementCount(m,+1);
}

void AntMeasurementBridge::onMeasurementDeleted(const fmp::Measurement::ConstPtr & m) {
	updateMeasurementCount(m,-1);
}

void AntMeasurementBridge::onIdentificationModified(fmp::Identification::ConstPtr identification) {
	buildCountForAnt(identification->Target());
}

QList<QStandardItem*> AntMeasurementBridge::buildAnt(const fmp::Ant::Ptr & ant) {
	if ( isActive() == false ) {
		return {};
	}
	QList<QStandardItem*> res = {new QStandardItem(AntGlobalModel::formatAntName(ant))};
	res.reserve(1 + d_experiment->MeasurementTypes().size() );
	for ( size_t i = 0; i < d_experiment->MeasurementTypes().size(); ++i ) {
		res.push_back(new QStandardItem("0"));
	}
	AntGlobalModel::setItemUserData(res[0],ant);
	for ( const auto & item : res ) {
		item->setEditable(false);
	}
	return res;
}


void AntMeasurementBridge::buildCountForAnt(const fmp::Ant::ConstPtr & ant) {
	auto antItem = d_model->itemFromAntID(ant->AntID());
	if ( isActive() == false || antItem == nullptr ) {
		return;
	}
	std::vector<fm::ComputedMeasurement> measurements;
	for ( size_t column = 1; column < d_model->columnCount(); ++column) {
		auto mtID = d_model->horizontalHeaderItem(column)->data().toInt();
		d_experiment->ComputeMeasurementsForAnt(measurements,ant->AntID(),mtID);
		d_model->item(antItem->row(),column)->setData(int(measurements.size()),Qt::DisplayRole);
	}
}

void AntMeasurementBridge::updateMeasurementCount(const fmp::Measurement::ConstPtr & m, int incrementValue) {
	if ( isActive() == false ) {
		return;
	}

	const auto & [tddURI,frameID,tagID,mtID ] = fmp::Measurement::DecomposeURI(m->URI());

	auto [headerItem,column] = headerForType(mtID);
	if ( headerItem == nullptr ) {
		return;
	}
	fmp::AntID antID(0);
	try {
		auto frameRef = d_experiment->TrackingDataDirectories().at(tddURI)->FrameReferenceAt(frameID);
		auto identification = d_experiment->CIdentifier().Identify(tagID,frameRef.Time());
		if ( !identification ) {
			return;
		}
		antID = identification->Target()->AntID();

	} catch (const std::exception & ) {
	}
	QStandardItem * antItem = d_model->itemFromAntID(antID);
	if ( antItem == nullptr ) {
		return;
	}
	auto valueItem = d_model->item(antItem->row(),column);
	valueItem->setData(valueItem->data(Qt::DisplayRole).toInt()+incrementValue,Qt::DisplayRole);
}

std::pair<QStandardItem*,int> AntMeasurementBridge::headerForType(quint32 mtID) const {
	for ( size_t column = 1; column < d_model->columnCount(); ++column) {
		auto item = d_model->horizontalHeaderItem(column);
		auto columnID = item->data().toInt();
		if ( columnID == mtID ) {
			return {item,column};
		} else if ( columnID > mtID ) {
			return {nullptr,column};
		}
	}
	return {nullptr,d_model->columnCount()};
}
