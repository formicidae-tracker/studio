#pragma once

class QAction;

struct NavigationAction {
	QAction * NextTag;
	QAction * PreviousTag;
	QAction * NextCloseUp;
	QAction * PreviousCloseUp;
	QAction * CopyCurrentTime;
};


class Navigatable {
public:

	virtual void SetUp(const NavigationAction & actions) = 0;
	virtual void TearDown(const NavigationAction & actions) = 0;

};
