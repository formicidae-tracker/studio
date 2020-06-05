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

	virtual void setUp(const NavigationAction & actions) = 0;
	virtual void tearDown(const NavigationAction & actions) = 0;

};
