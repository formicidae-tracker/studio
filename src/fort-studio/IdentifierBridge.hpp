#pragma once

#include <QObject>
#include <QStandardItemModel>

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Ant.hpp>

namespace fm = fort::myrmidon;
namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(fmp::Ant::Ptr)
Q_DECLARE_METATYPE(fmp::Ant::ConstPtr)
Q_DECLARE_METATYPE(fmp::Identification::Ptr)
Q_DECLARE_METATYPE(fmp::Identification::ConstPtr)

class IdentifierBridge : public QObject {
	Q_OBJECT
public:
	IdentifierBridge(QObject * parent);

	QAbstractItemModel * antModel() const;

	void setExperiment(const fmp::Experiment::Ptr & experiment);

signals:
	void antCreated(fmp::Ant::ConstPtr);
	void antDeleted(fm::Ant::ID);

	void identificationCreated(fmp::Identification::ConstPtr);
	void identificationAntPositionModified(fmp::Identification::ConstPtr);
	void identificationDeleted(fmp::Identification::ConstPtr);

	void antDisplayChanged(fm::Ant::ID,
	                       fmp::Color,
	                       fmp::Ant::DisplayState);

	void activated(bool);
public slots:
	fmp::Ant::Ptr createAnt();
	void removeAnt(fm::Ant::ID AID);

	fmp::Identification::Ptr addIdentification(fm::Ant::ID AID,
	                                           fmp::TagID TID,
	                                           fm::Time::ConstPtr & start,
	                                           fm::Time::ConstPtr & end);

	void deleteIdentification(const fmp::Identification::Ptr & ident);
private slots:

	void onItemChanged(QStandardItem *);
private:
	static QString formatIdentification(const fmp::Identification::Ptr & ident);
	static QString formatAntName(const fmp::Ant::Ptr & ant);

	static QIcon antDisplayColor(const fmp::Ant::Ptr & ant);
	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);

	QStandardItem * findAnt(fm::Ant::ID AID) const;

	fmp::Experiment::Ptr d_experiment;
	QStandardItemModel * d_model;
};
