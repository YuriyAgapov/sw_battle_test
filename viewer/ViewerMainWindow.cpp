#include "ViewerMainWindow.hpp"

#include "GameSceneWidget.hpp"
#include "IO/System/PrintDebug.hpp"

#include <Game/GameApp.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitHealed.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QTextBrowser>
#include <QTextEdit>
#include <QToolBar>

template <typename TCommand>
inline void subscribe(sw::ecs::EventDispatcher& dispatcher, QTextBrowser* logView)
{
	dispatcher.subscribe<TCommand>(
		[logView](const TCommand& data)
		{
			std::ostringstream stream;
			sw::printDebug(stream, data);
			logView->append(QString::fromStdString(stream.str()).trimmed());
		});
}

ViewerMainWindow::ViewerMainWindow(QWidget* parent) :
		QMainWindow{parent},
		app(std::make_unique<sw::game::GameApp>()),
		gameSceneWidget(new GameSceneWidget(app->getContext(), this)),
		logView(new QTextBrowser()),
		scenarioEdit(new QTextEdit())
{
	setWindowIcon(QIcon(":/images/hunter"));
	setCentralWidget(gameSceneWidget);

	QAction* openCommandsAction = new QAction(tr("Open"), this);
	connect(
		openCommandsAction,
		&QAction::triggered,
		this,
		[this](bool)
		{
			lastFileName = QFileDialog::getOpenFileName(this, tr("Select commands file"), qApp->applicationDirPath());

			QFile file(lastFileName);
			if (file.open(QIODevice::ReadOnly))
			{
				const QString scenario = file.readAll();
				scenarioEdit->setText(scenario);

				logView->clear();
				app->loadFromStr(scenario.toStdString());
			}
		});
	QAction* execScenarionCommandsAction = new QAction(tr("Exec scenario"), this);
	connect(
		execScenarionCommandsAction,
		&QAction::triggered,
		this,
		[this](bool)
		{
			logView->clear();
			app->loadFromStr(scenarioEdit->toPlainText().toStdString());
		});

	QAction* simulateStepAction = new QAction(tr("Simulate step"), this);
	connect(
		simulateStepAction,
		&QAction::triggered,
		this,
		[this](bool)
		{
			logView->clear();
			gameSceneWidget->clearTempLayer();
			// step
			app->getContext()->advance();
		});

	QToolBar* toolBar = addToolBar(tr("ToolBar"));
	toolBar->addAction(openCommandsAction);
	toolBar->addAction(execScenarionCommandsAction);
	toolBar->addAction(simulateStepAction);

	QDockWidget* logDock = new QDockWidget(this);
	logDock->setWindowTitle(tr("Turn log"));
	logDock->setWidget(logView);
	addDockWidget(Qt::BottomDockWidgetArea, logDock);

	QDockWidget* scenarioDock = new QDockWidget(this);
	scenarioDock->setWindowTitle(tr("Scenario"));
	scenarioDock->setWidget(scenarioEdit);
	addDockWidget(Qt::BottomDockWidgetArea, scenarioDock);

	subscribeToLogger();
}

ViewerMainWindow::~ViewerMainWindow() = default;

void ViewerMainWindow::subscribeToLogger()
{
	using namespace sw;
	auto& dispatcher = app->getContext()->getDispatcher();
	subscribe<io::MapCreated>(dispatcher, logView);
	subscribe<io::UnitSpawned>(dispatcher, logView);
	subscribe<io::UnitMoved>(dispatcher, logView);
	subscribe<io::MarchStarted>(dispatcher, logView);
	subscribe<io::MarchEnded>(dispatcher, logView);
	subscribe<io::UnitAttacked>(dispatcher, logView);
	subscribe<io::UnitHealed>(dispatcher, logView);
	subscribe<io::UnitDied>(dispatcher, logView);
}
