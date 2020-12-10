#pragma once



class Toolable {
public:

	virtual void setUp(QMainWindow * mainWindow) = 0;
	virtual void tearDown(QMainWindow * mainWindow) = 0;

};
