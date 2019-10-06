#pragma once

#include <QWidget>

#include <QMap>

#include "ExperimentController.hpp"

#include <myrmidon/priv/Experiment.hpp>

namespace Ui {
class FramePointerWidget;
}

class FramePointerWidget : public QWidget {
    Q_OBJECT
public:
    explicit FramePointerWidget(QWidget *parent = 0);
    ~FramePointerWidget();

	void setTitle(const QString & title);

signals:
	void framePointerUpdated(const fort::myrmidon::priv::FramePointerPtr & frame);

public slots:
	void onNewController(ExperimentController * controller);
	void onDataDirUpdated(const fort::myrmidon::priv::Experiment::TrackingDataDirectoryByPath & tdds);

	void on_comboBox_currentIndexChanged(int );
	void on_groupBox_toggled(bool);

	void on_spinBox_valueChanged(uint64_t);
private:
    Ui::FramePointerWidget *d_ui;

	ExperimentController * d_controller;
	bool d_inhibit;
};
