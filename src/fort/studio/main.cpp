#include <fort/studio/MainWindow.hpp>

#include <QApplication>

#include <QPointer>

#include <glog/logging.h>

int main(int argc, char ** argv) {

	FLAGS_logtostderr = false;
	FLAGS_log_prefix = false;
	::google::InitGoogleLogging(argv[0]);

	QCoreApplication::setOrganizationName("FORmicidae Tracker");
	QCoreApplication::setOrganizationDomain("formicidae-tracker.github.io");
	QCoreApplication::setApplicationName("FORT Studio");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


	QApplication fortStudio(argc,argv);



	MainWindow window;
	window.show();



	return fortStudio.exec();
}
