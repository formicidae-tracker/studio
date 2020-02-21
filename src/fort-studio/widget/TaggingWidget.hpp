#pragma once

#include <QWidget>
#include <QTreeWidgetItem>

#include "ExperimentController.hpp"

#include "SnapshotIndexer.hpp"
#include "TagExtractor.hpp"
#include "AntPoseEstimate.hpp"

#include <myrmidon/utils/FileSystem.hpp>

#include <myrmidon/priv/ForwardDeclaration.hpp>

namespace Ui {
class TaggingWidget;
}

class TaggingWidget : public QWidget {
    Q_OBJECT
public:
    explicit TaggingWidget(QWidget *parent = 0);
	~TaggingWidget();

public slots:
	Error save();

	void onNewController(ExperimentController * controller);
	void onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByURI &);
	void onTagFamilyChanged(fort::tags::Family f);

	void on_familySelector_currentIndexChanged(int);
	void on_thresholdBox_editingFinished();
	void on_tagList_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem*);
	void on_roiBox_valueChanged(int);

	void onNewSnapshots(const QVector<Snapshot::ConstPtr> &, size_t done);
	void on_snapshotViewer_antPoseEstimateUpdated(const AntPoseEstimate::Ptr & e);

	void on_addIdentButton_clicked();
	void on_newAntButton_clicked();
	void on_deletePoseButton_clicked();

	void onAntSelected(fort::myrmidon::Ant::ID);
	void onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr &);
	void onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr &);

	void onNewTrackedTags(const std::vector<fort::myrmidon::priv::TagID> & );

private:
	const static fs::path ESTIMATE_SAVE_PATH;
	const static char * GOOD_ICON;
	const static char * BAD_ICON;

	void clearIndexers();
	void updateButtonState();
	void updateUnusedCount();

	void updateIdentificationForCurrentFrame();
	fort::myrmidon::priv::IdentificationPtr
	updateIdentificationForFrame(fort::myrmidon::priv::TagID tag, const fort::myrmidon::priv::RawFrame & f);

    Ui::TaggingWidget *d_ui;
	ExperimentController * d_controller;
	std::map<fs::path,std::shared_ptr<SnapshotIndexer>> d_indexers;
	std::map<fs::path,std::shared_ptr<TagExtractor>>    d_extractors;

	std::unordered_map<fort::myrmidon::priv::TagID,QTreeWidgetItem*> d_tags;
	std::unordered_map<std::string,Snapshot::ConstPtr>               d_snapshots;
	std::set<fort::myrmidon::priv::TagID>                            d_used;

	std::set<fort::myrmidon::priv::TagID>                            d_allTrackedTags;


	std::map<fs::path,AntPoseEstimate::Ptr> d_estimates;

	fort::myrmidon::Ant::ID  d_selectedAnt;

};
