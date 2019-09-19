#include "MainWindow.hpp"

#include <QApplication>


int main(int argc, char ** argv) {
	QApplication fortStudio(argc,argv);

	QCoreApplication::setOrganizationName("FORmicidae Tracker");
	QCoreApplication::setOrganizationDomain("formicidae-tracker.github.io");
	QCoreApplication::setApplicationName("FORT Studio");



	MainWindow window;
	window.show();



	return fortStudio.exec();
}
