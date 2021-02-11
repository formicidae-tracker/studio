#pragma once

#include <QStandardItemModel>

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/MyrmidonTypes/Ant.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>
#include <fort/studio/MyrmidonTypes/Color.hpp>
#include <fort/studio/MyrmidonTypes/Time.hpp>

#include "Bridge.hpp"


class SelectedAntBridge;
class GlobalPropertyBridge;

class IdentifierBridge : public GlobalBridge {
	Q_OBJECT

public:
	IdentifierBridge(QObject * parent);
	virtual ~IdentifierBridge();

	QAbstractItemModel * model() const;

	fmp::Identification::ConstPtr identificationForIndex(const QModelIndex & index) const;

	void initialize(ExperimentBridge * experiment) override;


	fmp::IdentificationConstPtr identify(fmp::TagID tagID,
	                                     const fm::Time & time) const;

	bool freeRangeContaining(fm::Time & start,
	                         fm::Time & end,
	                         fmp::TagID tagID, const fm::Time & time) const;

	std::vector<fm::Ant::ID> unidentifiedAntAt(const fm::Time & time) const;
signals:
	void identificationCreated(fmp::Identification::ConstPtr);
	void identificationRangeModified(fmp::Identification::ConstPtr);
	void identificationSizeModified(fmp::Identification::ConstPtr);
	void identificationAntPositionModified(fmp::Identification::ConstPtr);
	void identificationDeleted(fmp::Identification::ConstPtr);

public slots:
	fmp::Identification::Ptr addIdentification(quint32 antID,
	                                           fmp::TagID tagID,
	                                           const fm::Time & start,
	                                           const fm::Time & end);

	void deleteIdentification(const fmp::Identification::ConstPtr & ident);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private slots:

	void onIdentificationItemChanged(QStandardItem *);

	void onDefaultTagSizeChanged(double tagSize);

private:
	QList<QStandardItem*> buildIdentification(const fmp::Identification::Ptr & identification);


	void rebuildModels();

	QStandardItem * findIdentification(const fmp::Identification::ConstPtr & identification) const;

	void onAntPositionUpdate(const fmp::Identification::ConstPtr & identification,
	                         const std::vector<fmp::AntPoseEstimateConstPtr> & estimations);

	void onStartItemChanged(QStandardItem * item);
	void onEndItemChanged(QStandardItem * item);
	void onSizeItemChanged(QStandardItem * item);


	const static int TAG_ID_COLUMN = 0;
	const static int ANT_ID_COLUMN = 1;
	const static int START_COLUMN  = 2;
	const static int END_COLUMN    = 3;
	const static int SIZE_COLUMN   = 4;
	const static int POSES_COLUMN  = 5;

	QStandardItemModel * d_model;
};
