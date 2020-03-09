#pragma once

#include <QStandardItemModel>

#include <myrmidon/priv/Experiment.hpp>

#include <fort-studio/MyrmidonTypes.hpp>
#include "Bridge.hpp"

class QModelIndex;
class QItemSelection;

class IdentifierBridge : public Bridge {
	Q_OBJECT
	Q_PROPERTY(quint32 numberSoloAnt
	           READ numberSoloAnt
	           NOTIFY numberSoloAntChanged)
	Q_PROPERTY(quint32 numberHiddenAnt
	           READ numberHiddenAnt
	           NOTIFY numberHiddenAntChanged)

public:
	IdentifierBridge(QObject * parent);

	QAbstractItemModel * antModel() const;

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isActive() const override;

	quint32 numberSoloAnt() const;

	quint32 numberHiddenAnt() const;

	fmp::IdentificationConstPtr identify(fmp::TagID tagID,
	                                     const fm::Time & time) const;
signals:
	void antCreated(fmp::Ant::ConstPtr);
	void antDeleted(quint32);

	void identificationCreated(fmp::Identification::ConstPtr);
	void identificationAntPositionModified(fmp::Identification::ConstPtr);
	void identificationDeleted(fmp::Identification::ConstPtr);

	void antDisplayChanged(quint32,
	                       fmp::Color,
	                       fmp::Ant::DisplayState);

	void antSelected(fmp::Ant::Ptr);

	void numberSoloAntChanged(quint32 numberSolo);
	void numberHiddenAntChanged(quint32 numberSolo);
public slots:
	fmp::Ant::Ptr createAnt();
	void deleteAnt(quint32 AID);
	void deleteSelection(const QItemSelection & selection);

	fmp::Identification::Ptr addIdentification(quint32,
	                                           fmp::TagID TID,
	                                           const fm::Time::ConstPtr & start,
	                                           const fm::Time::ConstPtr & end);

	void deleteIdentification(const fmp::Identification::Ptr & ident);
	void selectAnt(const QModelIndex & index);
	void setAntDisplayColor(const QItemSelection & selection,
	                        const QColor & color);

	void showAll();
	void unsoloAll();
private slots:

	void onItemChanged(QStandardItem *);
private:
	static QString formatIdentification(const fmp::Identification::Ptr & ident);
	static QString formatAntName(const fmp::Ant::Ptr & ant);

	static QIcon antDisplayColor(const fmp::Ant::Ptr & ant);

	void setAntDisplayState(QStandardItem * hideItem,
	                        QStandardItem * soloItem,
	                        const fmp::Ant::Ptr & ant,
	                        fmp::Ant::DisplayState ds);

	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);

	QStandardItem * findAnt(fm::Ant::ID AID) const;

	void doOnSelection(const QItemSelection & selection,
	                   const std::function<void (const fmp::Ant::Ptr & ant,
	                                             QStandardItem * item)> & toDo);

	const static int HIDE_COLUMN = 1;
	const static int SOLO_COLUMN = 2;

	fmp::Experiment::Ptr d_experiment;
	QStandardItemModel * d_model;
	quint32              d_numberSoloAnt,d_numberHiddenAnt;
};
