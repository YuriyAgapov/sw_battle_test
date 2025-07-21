#include "ViewerMainWindow.hpp"

#include <QApplication>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	ViewerMainWindow mainWindow;
	mainWindow.show();
	mainWindow.resize(640, 480);

	return app.exec();
}
