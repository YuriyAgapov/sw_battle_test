#pragma once

#include <QMainWindow>

class GameSceneWidget;
class QTextBrowser;
class QTextEdit;

namespace sw::game
{
	class GameApp;
}

class ViewerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ViewerMainWindow(QWidget* parent = nullptr);
	virtual ~ViewerMainWindow();

private:
	void subscribeToLogger();

	std::unique_ptr<sw::game::GameApp> app;
	GameSceneWidget* gameSceneWidget;
	QTextBrowser* logView;
	QTextEdit* scenarioEdit;
	QString lastFileName;
};
