#include "AntGlobalModel.hpp"

#include <fort/myrmidon/priv/Identifier.hpp>

#include "IdentifierBridge.hpp"

void AntGlobalModel::setItemUserData(QStandardItem * item,
                                     const fmp::Ant::Ptr & ant) {
	item->setData(QVariant::fromValue(ant));
}

fmp::Ant::Ptr AntGlobalModel::findAnt(const fmp::Experiment::Ptr & experiment,
                                      fm::Ant::ID antID) {
	if ( experiment == nullptr ) {
		return nullptr;
	}
	try {
		return experiment->Identifier()->Ants().at(antID);
	} catch ( const std::exception & ) {
		return nullptr;
	}
}

QString AntGlobalModel::formatAntName(const fmp::Ant::ConstPtr & ant) {
	QString res = ant->FormattedID().c_str();
	if ( ant->CIdentifications().empty() ) {
		return res + " <no-tags>";
	}
	std::set<fmp::TagID> tags;
	for ( const auto & i : ant->CIdentifications() ) {
		tags.insert(i->TagValue());
	}
	QString prefix = " ↤ {";
	for ( const auto & t : tags ) {
		res += prefix + fmp::FormatTagID(t).c_str();
		prefix = ",";
	}
	return res + "}";
}


AntGlobalModel::AntGlobalModel(QObject * parent)
	: QStandardItemModel(parent) {
	qRegisterMetaType<fmp::Ant::Ptr>();
}

AntGlobalModel::~AntGlobalModel() {
}

void AntGlobalModel::initialize(IdentifierBridge * identifier) {
	connect(identifier,&IdentifierBridge::identificationCreated,
	        this,&AntGlobalModel::onIdentificationModified);
	connect(identifier,&IdentifierBridge::identificationDeleted,
	        this,&AntGlobalModel::onIdentificationModified);

}

QStandardItem * AntGlobalModel::itemFromAntID(fm::Ant::ID antID, int column) const {
	for ( size_t i = 0; i < rowCount(); ++i ) {
		auto it = item(i,column);
		if ( it->data().value<fmp::Ant::Ptr>()->AntID() == antID ) {
			return it;
		}
	}
	return nullptr;
}

fm::Ant::ID AntGlobalModel::antIDFromIndex(const QModelIndex & index) const {
	const auto & ant = antFromIndex(index);
	if ( ant == nullptr ) {
		return 0;
	}
	return ant->AntID();
}

fmp::Ant::Ptr AntGlobalModel::antFromIndex(const QModelIndex & index) const {
	auto item = itemFromIndex(index);
	if ( item == nullptr ) {
		return nullptr;
	}
	return item->data().value<fmp::Ant::Ptr>();
}

void AntGlobalModel::onIdentificationModified(fmp::Identification::ConstPtr identification) {
	auto item = itemFromAntID(identification->Target()->AntID());
	if ( item == nullptr ) {
		return;
	}
	item->setText(formatAntName(item->data().value<fmp::Ant::Ptr>()));
}
