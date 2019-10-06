#pragma once

#include <QWidget>

#include "ExperimentController.hpp"

namespace Ui {
class AntIdentificationWidget;
}

class AntIdentificationWidget : public QWidget {
	Q_OBJECT
public:
	explicit AntIdentificationWidget(QWidget *parent = 0);
	~AntIdentificationWidget();

public slots:
	void onNewController(ExperimentController * controller);
	void on_startFrame_framePointerUpdated(const fort::myrmidon::priv::FramePointerPtr &);
	void on_endFrame_framePointerUpdated(const fort::myrmidon::priv::FramePointerPtr &);

private:
	Ui::AntIdentificationWidget * d_ui;

	ExperimentController * d_controller;

};
