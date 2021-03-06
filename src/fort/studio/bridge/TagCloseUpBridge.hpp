#pragma once

#include "Bridge.hpp"

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>
#include <fort/studio/MyrmidonTypes/TagCloseUp.hpp>
#include <fort/studio/MyrmidonTypes/Identification.hpp>
#include <fort/studio/MyrmidonTypes/Experiment.hpp>


class QAbstractItemModel;
class QStandardItemModel;
class IdentifierBridge;
class UniverseBridge;

class TagCloseUpBridge : public GlobalBridge {
	Q_OBJECT;
public:
	TagCloseUpBridge(QObject * parent);
	virtual ~TagCloseUpBridge();

	void initialize(ExperimentBridge * experiment) override;

	QAbstractItemModel * tagModel() const;

	fm::TagID tagIDFromIndex(const QModelIndex & index) const;

	QModelIndex indexForTag(fm::TagID tagID) const;

	const QVector<fmp::TagCloseUp::ConstPtr> & closeUpsForIndex(const QModelIndex & index) const;

	const QVector<fmp::TagCloseUp::ConstPtr> & closeUpsForTag(fm::TagID tagID) const;

	const QVector<fmp::TagCloseUp::ConstPtr> & closeUpsForAnt(fm::AntID antID) const;

private slots:

	void onTrackingDataDirectoryAdded(const fmp::TrackingDataDirectory::Ptr & tdd);
	void onTrackingDataDirectoryDeleted(const QString & uri);

	void onIdentificationModified(fmp::Identification::ConstPtr identification);

signals:
	void cleared();

	void closeUpsForTagChanged(uint32_t tagID,const QVector<fmp::TagCloseUp::ConstPtr> & closeUps);
	void closeUpsForAntChanged(uint32_t antID,const QVector<fmp::TagCloseUp::ConstPtr> & closeUps);

protected:
	void setUpExperiment() override;
	void tearDownExperiment() override;

private:
	void clear();
	void rebuild();

	void sort(QVector<fmp::TagCloseUp::ConstPtr> & closeUpsList);
	void count(const fm::TagID tagID);

	std::pair<std::set<fmp::TagID>,std::set<fmp::AntID>>
	addTrackingDataDirectory(const fmp::TrackingDataDirectory::Ptr & tdd);

	std::pair<std::set<fmp::TagID>,std::set<fmp::AntID>>
	removeTrackingDataDirectory(const QString & uri);

	std::pair<fm::TagID,fm::AntID> addCloseUp(const fmp::TagCloseUp::ConstPtr & closeUp);

	QStandardItemModel * d_tagModel;

	std::map<fm::TagID,QVector<fmp::TagCloseUp::ConstPtr>> d_tagsLists;
	std::map<fm::AntID,QVector<fmp::TagCloseUp::ConstPtr>> d_antsLists;

};
