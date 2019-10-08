#pragma once

#include <QWidget>
#include <QTreeWidgetItem>

#include "ExperimentController.hpp"

#include "SnapshotIndexer.hpp"
#include "TagExtractor.hpp"
#include "AntPoseEstimate.hpp"

#include <filesystem>

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
	void onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &);
	void on_familySelector_activated(int);
	void on_thresholdBox_editingFinished();
	void on_tagList_itemActivated(QTreeWidgetItem *, int);
	void on_roiBox_valueChanged(int);

	void onNewSnapshots(const QVector<Snapshot::ConstPtr> &, size_t done);
	void on_snapshotViewer_antPoseEstimateUpdated(const AntPoseEstimate::Ptr & e);

	void on_addIdentButton_clicked();
	void on_newAntButton_clicked();
	void on_deletePoseButton_clicked();

	void onAntSelected(fort::myrmidon::Ant::ID);
	void onIdentificationCreated(const fort::myrmidon::priv::IdentificationPtr &);
	void onIdentificationDeleted(const fort::myrmidon::priv::IdentificationPtr &);

	void onNewTrackedTags(const std::vector<uint32_t> & );

private:
	const static std::filesystem::path ESTIMATE_SAVE_PATH;
	void clearIndexers();
	void updateButtonState();
	void updateUnusedCount();

	void updateIdentificationForCurrentFrame();
	fort::myrmidon::priv::IdentificationPtr
	updateIdentificationForFrame(uint32_t tag, const fort::myrmidon::priv::FramePointer & f);

    Ui::TaggingWidget *d_ui;
	ExperimentController * d_controller;
	std::map<std::filesystem::path,std::shared_ptr<SnapshotIndexer>> d_indexers;
	std::map<std::filesystem::path,std::shared_ptr<TagExtractor>>    d_extractors;

	std::unordered_map<uint32_t,QTreeWidgetItem*>      d_tags;
	std::unordered_map<std::string,Snapshot::ConstPtr> d_snapshots;
	std::set<uint32_t>                                 d_used;

	std::set<uint32_t>                                 d_allTrackedTags;


	std::map<std::filesystem::path,AntPoseEstimate::Ptr> d_estimates;

	fort::myrmidon::Ant::ID  d_selectedAnt;

};
