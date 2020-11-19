#pragma once

#include <QStandardItemModel>

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/MyrmidonTypes/Ant.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>
#include <fort/studio/MyrmidonTypes/Color.hpp>
#include <fort/studio/MyrmidonTypes/Time.hpp>

#include "Bridge.hpp"

class QModelIndex;
class QItemSelection;

class SelectedAntBridge;

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
	virtual ~IdentifierBridge();

	QAbstractItemModel * antModel() const;

	QAbstractItemModel * identificationsModel() const;

	void setExperiment(const fmp::Experiment::Ptr & experiment);

	bool isActive() const override;

	quint32 numberSoloAnt() const;

	quint32 numberHiddenAnt() const;

	fmp::IdentificationConstPtr identify(fmp::TagID tagID,
	                                     const fm::Time & time) const;

	bool freeRangeContaining(fm::Time::ConstPtr & start,
	                         fm::Time::ConstPtr & end,
	                         fmp::TagID tagID, const fm::Time & time) const;

	SelectedAntBridge * selectedAnt() const;

	fmp::Ant::ConstPtr ant(fmp::AntID antID) const;



signals:
	void antCreated(fmp::Ant::ConstPtr);
	void antDeleted(quint32);

	void identificationCreated(fmp::Identification::ConstPtr);
	void identificationRangeModified(fmp::Identification::ConstPtr);
	void identificationSizeModified(fmp::Identification::ConstPtr);
	void identificationAntPositionModified(fmp::Identification::ConstPtr);
	void identificationDeleted(fmp::Identification::ConstPtr);

	void antDisplayChanged(quint32,
	                       fm::Color,
	                       fmp::Ant::DisplayState);

	void numberSoloAntChanged(quint32 numberSolo);
	void numberHiddenAntChanged(quint32 numberSolo);
public slots:
	fmp::Ant::Ptr createAnt();
	void deleteAnt(quint32 antID);
	void deleteSelection(const QItemSelection & selection);

	fmp::Identification::Ptr addIdentification(quint32 antID,
	                                           fmp::TagID tagID,
	                                           const fm::Time::ConstPtr & start,
	                                           const fm::Time::ConstPtr & end);

	void deleteIdentification(const fmp::Identification::Ptr & ident);
	void selectAnt(const QModelIndex & index);
	void setAntDisplayColor(const QItemSelection & selection,
	                        const QColor & color);

	void showAll();
	void unsoloAll();
private slots:

	void onAntItemChanged(QStandardItem *);
	void onIdentificationItemChanged(QStandardItem *);



private:
	static QString formatAntName(const fmp::Ant::Ptr & ant);

	static QIcon antDisplayColor(const fmp::Ant::Ptr & ant);

	void setAntDisplayState(QStandardItem * hideItem,
	                        QStandardItem * soloItem,
	                        const fmp::Ant::Ptr & ant,
	                        fmp::Ant::DisplayState ds);

	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);

	QList<QStandardItem*> buildIdentification(const fmp::Identification::Ptr & identification);


	void rebuildModels();

	QStandardItem * findAnt(fmp::AntID antID) const;

	QStandardItem * findIdentification(const fmp::Identification::ConstPtr & identification) const;

	void doOnSelection(const QItemSelection & selection,
	                   const std::function<void (const fmp::Ant::Ptr & ant,
	                                             QStandardItem * item)> & toDo);

	void onAntPositionUpdate(const fmp::Identification::ConstPtr & identification,
	                         const std::vector<fmp::AntPoseEstimateConstPtr> & estimations);


	void onStartItemChanged(QStandardItem * item);
	void onEndItemChanged(QStandardItem * item);
	void onSizeItemChanged(QStandardItem * item);




	const static int HIDE_COLUMN = 1;
	const static int SOLO_COLUMN = 2;

	const static int TAG_ID_COLUMN = 0;
	const static int ANT_ID_COLUMN = 1;
	const static int START_COLUMN  = 2;
	const static int END_COLUMN    = 3;
	const static int SIZE_COLUMN   = 4;
	const static int POSES_COLUMN  = 5;

	fmp::Experiment::Ptr d_experiment;
	QStandardItemModel * d_antModel, * d_identificationModel;

	quint32              d_numberSoloAnt,d_numberHiddenAnt;
	SelectedAntBridge  * d_selectedAnt;
};
