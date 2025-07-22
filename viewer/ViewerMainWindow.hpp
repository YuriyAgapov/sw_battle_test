#pragma once

#include <QMainWindow>

class QTextBrowser;
class GameSceneWidget;

namespace sw::game {class GameApp;}

class ViewerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ViewerMainWindow(QWidget* parent = nullptr);
	virtual ~ViewerMainWindow();

private:
	void subscribeLog();

	std::unique_ptr<sw::game::GameApp> app;
	GameSceneWidget* gameSceneWidget;
	QTextBrowser* logView;
	QString lastFileName;

};
