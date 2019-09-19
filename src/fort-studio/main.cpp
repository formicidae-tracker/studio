#include "MainWindow.hpp"

#include <QApplication>


int main(int argc, char ** argv) {
	QApplication antStudio(argc,argv);
	MainWindow window;
	window.show();

	return antStudio.exec();
}
