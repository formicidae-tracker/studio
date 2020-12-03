#pragma once

#include <QStandardItemModel>

#include <fort/studio/MyrmidonTypes/Experiment.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>
#include <fort/studio/MyrmidonTypes/Ant.hpp>

class QStandardItemModel;
class IdentifierBridge;

class AntGlobalModel : public QStandardItemModel {
	Q_OBJECT
public:
	static void setItemUserData(QStandardItem * item, const fmp::Ant::Ptr & ant);

	static fmp::Ant::Ptr findAnt(const fmp::Experiment::Ptr & experiment,
	                             fm::Ant::ID antID);

	static QString formatAntName(const fmp::Ant::ConstPtr & ant);


	explicit AntGlobalModel(QObject * parent);
	virtual ~AntGlobalModel();

	void initialize(IdentifierBridge * identifier);

	QStandardItem * itemFromAntID(fm::Ant::ID antID, int column = 0) const;

	fm::Ant::ID antIDFromIndex(const QModelIndex & index) const;
	fmp::Ant::Ptr antFromIndex(const QModelIndex & index) const;


protected slots:

	void onIdentificationModified(fmp::Identification::ConstPtr);

};
