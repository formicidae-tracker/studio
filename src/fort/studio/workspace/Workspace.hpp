#pragma once

#include <QWidget>

class QAction;
class QMainWindow;
class ExperimentBridge;

struct NavigationAction {
	QAction * NextTag;
	QAction * PreviousTag;
	QAction * NextCloseUp;
	QAction * PreviousCloseUp;
	QAction * CopyCurrentTime;

	QAction * JumpToTime;
};


class Workspace : public QWidget {
	Q_OBJECT
public:
	explicit Workspace(bool showAntSelector, QWidget * parent);
	virtual ~Workspace();

	bool showAntSelector() const;

	virtual void initialize(ExperimentBridge * experiment) = 0;
	virtual void setUp(QMainWindow * main,const NavigationAction & actions) = 0;
	virtual void tearDown(QMainWindow * maina,const NavigationAction & actions) = 0;

private:
	bool d_showAntSelector;
};
