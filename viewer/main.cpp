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
#include <QMainWindow>
#include <QPainter>
#include <QTextBrowser>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

namespace sw::view
{
	static QPointF toPointF(const uint32_t x, const uint32_t y)
	{
		return QPointF{static_cast<qreal>(x), static_cast<qreal>(y)};
	}

	class GameViewWidget : public QWidget
	{
	public:
		GameViewWidget(const std::shared_ptr<ecs::Context>& context, QWidget* parent) :
				QWidget(parent),
				context(context)
		{
			context->getDispatcher().subscribe<io::MapCreated>([this](const io::MapCreated& event)
															   { setSceneSize(toPointF(event.width, event.height)); });
			context->getDispatcher().subscribe<io::UnitSpawned>(
				[this](const io::UnitSpawned& event)
				{
					items.emplace(event.unitId, Item{event.unitType, toPointF(event.x, event.y)});
					repaint();
				});
			context->getDispatcher().subscribe<io::UnitMoved>(
				[this](const io::UnitMoved& event)
				{
					items[event.unitId].pos = toPointF(event.x, event.y);
					repaint();
				});
			context->getDispatcher().subscribe<io::MarchStarted>(
				[this](const io::MarchStarted& event)
				{
					Item& item = items[event.unitId];
					item.pos = toPointF(event.x, event.y);
					item.endPos = toPointF(event.targetX, event.targetY);
					repaint();
				});
			context->getDispatcher().subscribe<io::MarchEnded>(
				[this](const io::MarchEnded& event)
				{
					items[event.unitId].pos = toPointF(event.x, event.y);
					repaint();
				});
		}

		virtual void paintEvent(QPaintEvent* event) final
		{
			QPainter painter(this);

			// draw grid
			painter.setPen(Qt::gray);
			for (qreal x = 0.0; x <= sceneSize.x(); ++x)
			{
				painter.drawLine(sceneTransform.map(QPointF(x, 0.0)), sceneTransform.map(QPointF(x, sceneSize.y())));
			}

			for (qreal y = 0.0; y <= sceneSize.y(); ++y)
			{
				painter.drawLine(sceneTransform.map(QPointF(0.0, y)), sceneTransform.map(QPointF(sceneSize.x(), y)));
			}

			static std::unordered_map<std::string, QBrush> styles{
																  {"Swordsman", QBrush(Qt::red)}, {"Hunter", QBrush(Qt::green)}};

			// command layer
			painter.setPen(Qt::NoPen);
			for (const auto& [_, item] : items)
			{
				if (!item.endPos.isNull())
				{
					painter.setBrush(Qt::gray);
					painter.drawEllipse(sceneTransform.map(item.endPos + QPointF(0.5, 0.5)), 0.4 * sceneScale, 0.4 * sceneScale);
				}
			}

			// unit layer
			painter.setPen(Qt::black);
			for (const auto& [_, item] : items)
			{
				painter.setBrush(styles[item.type]);
				painter.drawEllipse(sceneTransform.map(item.pos + QPointF(0.5, 0.5)), 0.3 * sceneScale, 0.3 * sceneScale);
			}
		}

		virtual void resizeEvent(QResizeEvent* event) final
		{
			QWidget::resizeEvent(event);
			updateViewTransform();
		}

	private:
		void updateViewTransform()
		{
			if (width() <= 0 || height() <= 0 || sceneSize.x() <= 0 || sceneSize.y() <= 0)
			{
				return;
			}

			const QPointF widgetSize{static_cast<qreal>(width()), static_cast<qreal>(height())};

			const qreal scaleX = widgetSize.x() / sceneSize.x();
			const qreal scaleY = widgetSize.y() / sceneSize.y();
			sceneScale = std::min(scaleX, scaleY);

			const QPointF scaledSceneSize = sceneSize * sceneScale;
			const QPointF offset = (widgetSize - scaledSceneSize) / 2.0;

			sceneTransform = QTransform();
			sceneTransform.translate(offset.x(), offset.y());
			sceneTransform.scale(sceneScale, sceneScale);
		}

		void setSceneSize(const QPointF& inSceneSize)
		{
			sceneSize = inSceneSize;
			updateViewTransform();
			repaint();
		}

		std::shared_ptr<ecs::Context> context;

		QPointF sceneSize;
		QTransform sceneTransform;
		qreal sceneScale = 1.0;

		struct Item
		{
			std::string type;
			QPointF pos;
			QPointF endPos;
		};

		std::unordered_map<uint32_t, Item> items;
	};
}

class MainWindow : public QMainWindow
{
public:
	MainWindow() :
			GameViewWidget(new sw::view::GameViewWidget(app.getContext(), this))
			, LogView(new QTextBrowser())
	{
		setCentralWidget(GameViewWidget);

		QAction* openCommandsAction = new QAction(tr("Open"), this);
		connect(
			openCommandsAction,
			&QAction::triggered,
			this,
			[this](bool)
			{
				const QString fileName = QFileDialog::getOpenFileName(this, tr("Select commands file"), qApp->applicationDirPath());
				app.loadFromFile(fileName.toStdString());
			});

		QAction* simulateStepAction = new QAction(tr("Simulate step"), this);
		connect(
			simulateStepAction,
			&QAction::triggered,
			this,
			[this](bool)
			{
				LogView->clear();
				// step
				app.getContext()->advance();
			});

		QToolBar* toolBar = addToolBar(tr("ToolBar"));
		toolBar->addAction(openCommandsAction);
		toolBar->addAction(simulateStepAction);

		QDockWidget* logDock = new QDockWidget(this);
		logDock->setWindowTitle(tr("Turn log"));
		logDock->setWidget(LogView);
		addDockWidget(Qt::RightDockWidgetArea, logDock);

		subscribeLog();
	}

private:
	void subscribeLog()
	{
		using namespace sw;
		auto& dispatcher = app.getContext()->getDispatcher();
		dispatcher.subscribe<io::MapCreated>([this](const io::MapCreated& event)
														   { LogView->append(QString("MapCreated %1x%2").arg(event.width).arg(event.height));});
		dispatcher.subscribe<io::UnitSpawned>(
			[this](const io::UnitSpawned& event)
			{
				LogView->append(QString("UnitSpawned id=%1, type=%2, x=%3, y=%4").arg(event.unitId).arg(event.unitType).arg(event.x).arg(event.y));
			});
		dispatcher.subscribe<io::UnitMoved>(
			[this](const io::UnitMoved& event)
			{
				LogView->append(QString("UnitMoved id=%1, x=%2, y=%3").arg(event.unitId).arg(event.x).arg(event.y));
			});
		dispatcher.subscribe<io::MarchStarted>(
			[this](const io::MarchStarted& event)
			{
				LogView->append(QString("MarchStarted id=%1, x=%2, y=%3, targetX=%4, targetY=%5").arg(event.unitId).arg(event.x).arg(event.y).arg(event.targetX).arg(event.targetY));
			});
		dispatcher.subscribe<io::MarchEnded>(
			[this](const io::MarchEnded& event)
			{
				LogView->append(QString("MarchEnded id=%1, x=%2, y=%3").arg(event.unitId).arg(event.x).arg(event.y));
			});
		dispatcher.subscribe<io::UnitAttacked>(
			[this](const io::UnitAttacked& event)
			{
				LogView->append(QString("UnitAttacked attackerUnitId=%1, targetUnitId=%2, v=%3, targetHp=%4").arg(event.attackerUnitId).arg(event.targetUnitId).arg(event.damage).arg(event.targetHp));
			});
		dispatcher.subscribe<io::UnitDied>(
			[this](const io::UnitDied& event)
			{
				LogView->append(QString("UnitDied id=%1").arg(event.unitId));
			});
	}
	sw::game::GameApp app;
	sw::view::GameViewWidget* GameViewWidget;
	QTextBrowser* LogView;
};

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}
