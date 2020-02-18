#pragma once

#include <QStandardItemModel>

#include <myrmidon/priv/Experiment.hpp>

#include "MyrmidonTypes.hpp"
#include "Bridge.hpp"

class IdentifierBridge : public Bridge {
	Q_OBJECT
public:
	IdentifierBridge(QObject * parent);

	QAbstractItemModel * antModel() const;

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isActive() const override;

signals:
	void antCreated(fmp::Ant::ConstPtr);
	void antDeleted(quint32);

	void identificationCreated(fmp::Identification::ConstPtr);
	void identificationAntPositionModified(fmp::Identification::ConstPtr);
	void identificationDeleted(fmp::Identification::ConstPtr);

	void antDisplayChanged(quint32,
	                       fmp::Color,
	                       fmp::Ant::DisplayState);

public slots:
	fmp::Ant::Ptr createAnt();
	void removeAnt(quint32 AID);

	fmp::Identification::Ptr addIdentification(quint32,
	                                           fmp::TagID TID,
	                                           const fm::Time::ConstPtr & start,
	                                           const fm::Time::ConstPtr & end);

	void deleteIdentification(const fmp::Identification::Ptr & ident);
private slots:

	void onItemChanged(QStandardItem *);
private:
	static QString formatIdentification(const fmp::Identification::Ptr & ident);
	static QString formatAntName(const fmp::Ant::Ptr & ant);

	static QIcon antDisplayColor(const fmp::Ant::Ptr & ant);

	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);

	QStandardItem * findAnt(fm::Ant::ID AID) const;

	const static int HIDE_COLUMN = 1;
	const static int SOLO_COLUMN = 2;

	fmp::Experiment::Ptr d_experiment;
	QStandardItemModel * d_model;
};
