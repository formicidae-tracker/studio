#pragma once

#include <QObject>
#include <QStandardItemModel>

#include <myrmidon/priv/Experiment.hpp>
#include <myrmidon/priv/Ant.hpp>

namespace fm = fort::myrmidon;
namespace fmp = fort::myrmidon::priv;

Q_DECLARE_METATYPE(fmp::Ant::Ptr)
Q_DECLARE_METATYPE(fmp::Identification::Ptr)


class IdentifierBridge : public QObject {
	Q_OBJECT
public:
	IdentifierBridge(QObject * parent);

	QAbstractItemModel * antModel() const;

	void SetExperiment(const fmp::Experiment::Ptr & experiment);

signals:
	void antCreated(fmp::Ant::Ptr);
	void antDeleted(fm::Ant::ID);

	void identificationCreated(fmp::Identification::Ptr);
	void identificationAntPositionModified(fmp::Identification::Ptr);
	void identificationDeleted(fmp::Identification::Ptr);

public slots:
	fmp::Ant::Ptr createAnt();
	void removeAnt(fm::Ant::ID AID);

	fmp::Identification::Ptr addIdentification(fm::Ant::ID AID,
	                                           fmp::TagID TID,
	                                           fm::Time::ConstPtr & start,
	                                           fm::Time::ConstPtr & end);

	void deleteIdentification(const fmp::Identification::Ptr & ident);

private:
	static QString formatIdentification(const fmp::Identification::Ptr & ident);
	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);
	QList<QStandardItem*> buildIdentification(const fmp::Identification::Ptr & ant);

	void rebuildIdentifications(QStandardItem * toItem,
	                            const fmp::Ant::Ptr & ant);

	QStandardItem * findAnt(fm::Ant::ID AID) const;

	fmp::Experiment::Ptr d_experiment;
	QStandardItemModel * d_model;
};
