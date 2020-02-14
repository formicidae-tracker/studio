#include "MainWindow.hpp"

#include <QApplication>

#include "Logger.hpp"

#include <QPointer>

int main(int argc, char ** argv) {

	QCoreApplication::setOrganizationName("FORmicidae Tracker");
	QCoreApplication::setOrganizationDomain("formicidae-tracker.github.io");
	QCoreApplication::setApplicationName("FORT Studio");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


	QApplication fortStudio(argc,argv);



	MainWindow window;
	window.show();



	return fortStudio.exec();
}
