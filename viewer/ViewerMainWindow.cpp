#include "ViewerMainWindow.hpp"

#include "GameSceneWidget.hpp"

#include <Game/GameApp.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QTextBrowser>
#include <QToolBar>

ViewerMainWindow::ViewerMainWindow(QWidget* parent) :
		QMainWindow{parent},
		app(std::make_unique<sw::game::GameApp>()),
		gameSceneWidget(new GameSceneWidget(app->getContext(), this)),
		logView(new QTextBrowser())
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
			logView->clear();
			lastFileName = QFileDialog::getOpenFileName(this, tr("Select commands file"), qApp->applicationDirPath());
			app->loadFromFile(lastFileName.toStdString());
		});
	QAction* reopenCommandsAction = new QAction(tr("Reopen"), this);
	connect(
		reopenCommandsAction,
		&QAction::triggered,
		this,
		[this](bool)
		{
			logView->clear();
			app->loadFromFile(lastFileName.toStdString());
		});

	QAction* simulateStepAction = new QAction(tr("Simulate step"), this);
	connect(
		simulateStepAction,
		&QAction::triggered,
		this,
		[this](bool)
		{
			logView->clear();
			// step
			app->getContext()->advance();
		});

	QToolBar* toolBar = addToolBar(tr("ToolBar"));
	toolBar->addAction(openCommandsAction);
	toolBar->addAction(reopenCommandsAction);
	toolBar->addAction(simulateStepAction);

	QDockWidget* logDock = new QDockWidget(this);
	logDock->setWindowTitle(tr("Turn log"));
	logDock->setWidget(logView);
	addDockWidget(Qt::BottomDockWidgetArea, logDock);

	subscribeLog();
}

ViewerMainWindow::~ViewerMainWindow() = default;

void ViewerMainWindow::subscribeLog()
{
	using namespace sw;
	auto& dispatcher = app->getContext()->getDispatcher();
	dispatcher.subscribe<io::MapCreated>(
		[this](const io::MapCreated& event)
		{
			//
			logView->append(QString("MapCreated %1x%2").arg(event.width).arg(event.height));
		});
	dispatcher.subscribe<io::UnitSpawned>(
		[this](const io::UnitSpawned& event)
		{
			//
			logView->append(QString("UnitSpawned id=%1, type=%2, x=%3, y=%4")
								.arg(event.unitId)
								.arg(event.unitType)
								.arg(event.x)
								.arg(event.y));
		});
	dispatcher.subscribe<io::UnitMoved>(
		[this](const io::UnitMoved& event)
		{
			//
			logView->append(QString("UnitMoved id=%1, x=%2, y=%3").arg(event.unitId).arg(event.x).arg(event.y));
		});
	dispatcher.subscribe<io::MarchStarted>(
		[this](const io::MarchStarted& event)
		{
			//
			logView->append(QString("MarchStarted id=%1, x=%2, y=%3, targetX=%4, targetY=%5")
								.arg(event.unitId)
								.arg(event.x)
								.arg(event.y)
								.arg(event.targetX)
								.arg(event.targetY));
		});
	dispatcher.subscribe<io::MarchEnded>(
		[this](const io::MarchEnded& event)
		{
			//
			logView->append(QString("MarchEnded id=%1, x=%2, y=%3").arg(event.unitId).arg(event.x).arg(event.y));
		});
	dispatcher.subscribe<io::UnitAttacked>(
		[this](const io::UnitAttacked& event)
		{
			//
			logView->append(QString("UnitAttacked attackerUnitId=%1, targetUnitId=%2, damage=%3, targetHp=%4")
								.arg(event.attackerUnitId)
								.arg(event.targetUnitId)
								.arg(event.damage)
								.arg(event.targetHp));
		});
	dispatcher.subscribe<io::UnitDied>(
		[this](const io::UnitDied& event)
		{
			//
			logView->append(QString("UnitDied id=%1").arg(event.unitId));
		});
}
