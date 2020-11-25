#pragma once

#include "Bridge.hpp"

class QAbstractItemModel;
class AntGlobalModel;
class QStandardItemModel;
class QStandardItem;
class QItemSelection;

#include <fort/studio/MyrmidonTypes/Ant.hpp>

class AntDisplayBridge : public GlobalBridge {
	Q_OBJECT
	Q_PROPERTY(quint32 numberSoloAnt
	           READ numberSoloAnt
	           NOTIFY numberSoloAntChanged)
	Q_PROPERTY(quint32 numberHiddenAnt
	           READ numberHiddenAnt
	           NOTIFY numberHiddenAntChanged)

public:
	AntDisplayBridge(QObject * parent);
	virtual ~AntDisplayBridge();

	void initialize(ExperimentBridge * experiment) override;


	QAbstractItemModel * model() const;

	quint32 numberSoloAnt() const;

	quint32 numberHiddenAnt() const;

	fm::Ant::ID antIDForIndex(const QModelIndex & index) const;

	std::pair<fmp::Ant::DisplayState,fm::Color> displayStatusAndColor(fm::Ant::ID antID) const;


signals:

	void antDisplayChanged(quint32,
	                       fm::Color,
	                       fmp::Ant::DisplayState);

	void numberSoloAntChanged(quint32 numberSolo);
	void numberHiddenAntChanged(quint32 numberSolo);


public slots:
	void setAntDisplayColor(quint32 antID,
	                        const QColor & color);

	void setAntDisplayColor(const QModelIndex & index,
	                        const QColor & color);


	void showAll();
	void unsoloAll();

protected:
	void setUpExperiment();
	void tearDownExperiment();

private slots:
	void onAntItemChanged(QStandardItem * item);

	void onAntCreated(quint32 antID);
	void onAntDeleted(quint32 antID);


private:
	static QIcon antDisplayColor(const fmp::Ant::ConstPtr & ant);

	void setAntDisplayColor(QStandardItem * item,
	                        const QColor & color);


	void setAntDisplayState(QStandardItem * hideItem,
	                        QStandardItem * soloItem,
	                        const fmp::Ant::Ptr & ant,
	                        fmp::Ant::DisplayState ds);

	QList<QStandardItem*> buildAnt(const fmp::Ant::Ptr & ant);

	void doOnSelection(const QItemSelection & selection,
	                   const std::function<void (const fmp::Ant::Ptr & ant,
	                                             QStandardItem * item)> & toDo);

	void clear();
	void rebuildModel();

	const static int HIDE_COLUMN = 1;
	const static int SOLO_COLUMN = 2;


	AntGlobalModel * d_model;
	quint32          d_numberSoloAnt,d_numberHiddenAnt;

};
