#include <QApplication>


#include "CapsuleCollisionDetecter.hpp"

int main(int argc,char ** argv) {
	QApplication app(argc,argv);

	CapsuleCollisionDetecter mainWindow;
	mainWindow.show();

	return app.exec();
}
