#pragma once

#include <QWidget>

class QAction;
class QMainWindow;
class QToolBar;
class ExperimentBridge;

struct NavigationAction {
	QAction * NextTag;
	QAction * PreviousTag;
	QAction * NextCloseUp;
	QAction * PreviousCloseUp;
	QAction * CopyCurrentTime;

	QAction * JumpToTime;

	QToolBar * NavigationToolBar;
};


class Workspace : public QWidget {
	Q_OBJECT
public:
	explicit Workspace(bool showAntSelector, QWidget * parent);
	virtual ~Workspace();

	virtual void initialize(QMainWindow * main,ExperimentBridge * experiment) = 0;
	virtual void setUp(const NavigationAction & actions) = 0;
	virtual void tearDown(const NavigationAction & actions) = 0;
};
