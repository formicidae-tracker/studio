#pragma once

#include <QWidget>
#include <QTreeWidgetItem>

#include "ExperimentController.hpp"

#include "SnapshotIndexer.hpp"



namespace Ui {
class TaggingWidget;
}

class TaggingWidget : public QWidget {
    Q_OBJECT
public:
    explicit TaggingWidget(QWidget *parent = 0);
	~TaggingWidget();


public slots:
	void onNewController(ExperimentController * controller);
	void onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath &);

	void on_familySelector_activated(int);
	void on_thresholdBox_editingFinished();
	void on_tagList_itemActivated(QTreeWidgetItem *, int);


	void onNewSnapshots(const QVector<Snapshot::ConstPtr> &, size_t done);

private:
	void clearIndexers();

    Ui::TaggingWidget *d_ui;
	ExperimentController * d_controller;
	std::map<std::filesystem::path,std::shared_ptr<SnapshotIndexer>> d_indexers;

	std::unordered_map<uint32_t,QTreeWidgetItem*>      d_tags;
	std::unordered_map<std::string,Snapshot::ConstPtr> d_snapshots;

};
