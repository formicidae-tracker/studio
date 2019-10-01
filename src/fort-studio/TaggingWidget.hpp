#pragma once

#include <QWidget>

#include "ExperimentController.hpp"

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


private:
    Ui::TaggingWidget *d_ui;
	ExperimentController * d_controller;

	QMap<QString,QFutureWatcher> d_crawlers;
};
