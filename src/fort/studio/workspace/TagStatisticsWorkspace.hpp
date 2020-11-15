#pragma once

#include "Workspace.hpp"

namespace Ui {
class TagStatisticsWorkspace;
}

class TagStatisticsWorkspace : public Workspace {
	Q_OBJECT
public:
	explicit TagStatisticsWorkspace(QWidget *parent = 0);
	virtual ~TagStatisticsWorkspace();

protected:
	void initialize(QMainWindow * main,ExperimentBridge * experiment) override;
	void setUp(const NavigationAction & actions) override;
	void tearDown(const NavigationAction & actions) override;

private:
	Ui::TagStatisticsWorkspace * d_ui;
};
