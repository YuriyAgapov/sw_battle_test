#include "GameSceneWidget.hpp"

#include <ECS/Context.hpp>
#include <Game/Components/ViewerComponent.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitHealed.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <Math/Algo.hpp>
#include <QPainter>

using namespace sw;

static QPointF toPointF(const uint32_t x, const uint32_t y)
{
	return QPointF{static_cast<qreal>(x), static_cast<qreal>(y)};
}

GameSceneWidget::GameSceneWidget(const std::shared_ptr<sw::ecs::Context>& context, QWidget* parent) :
		QWidget(parent),
		context(context)
{
	context->getDispatcher().subscribe<io::MapCreated>(
		[this](const io::MapCreated& event)
		{
			died.clear();
			items.clear();
			setSceneSize(toPointF(event.width, event.height));
		});
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
	context->getDispatcher().subscribe<io::UnitAttacked>(
		[this](const io::UnitAttacked& event)
		{
			attacks.emplace_back(QLineF(items[event.attackerUnitId].pos, items[event.targetUnitId].pos), 1);
			repaint();
		});
	context->getDispatcher().subscribe<io::UnitHealed>(
		[this](const io::UnitHealed& event)
		{
			attacks.emplace_back(QLineF(items[event.attackerUnitId].pos, items[event.targetUnitId].pos), 2);
			repaint();
		});
	context->getDispatcher().subscribe<io::UnitDied>(
		[this](const io::UnitDied& event)
		{
			Item unit = std::move(items[event.unitId]);
			unit.type = "died";
			items.erase(event.unitId);
			died.emplace_back(std::move(unit));
			repaint();
		});
}

QSize GameSceneWidget::sizeHint()
{
	return QSize(640, 480);
}

void GameSceneWidget::paintEvent(QPaintEvent* event)
{
	static std::unordered_map<std::string, QPixmap> styles{
		{"Swordsman", QPixmap(":/images/swordsman")},
		{"Hunter", QPixmap(":/images/hunter")},
		{"Tower", QPixmap(":/images/tower")},
		{"Healer", QPixmap(":/images/healer")},
		{"died", QPixmap(":/images/died")}};

	const QPointF pixOffset{0.1, 0.1};

	QPainter painter(this);

	// draw grid
	painter.setPen(Qt::gray);
	for (uint32_t x = 0.0; x <= sceneSize.x(); ++x)
	{
		painter.drawLine(sceneTransform.map(QPointF(x, 0.0)), sceneTransform.map(QPointF(x, sceneSize.y())));
	}

	for (uint32_t y = 0.0; y <= sceneSize.y(); ++y)
	{
		painter.drawLine(sceneTransform.map(QPointF(0.0, y)), sceneTransform.map(QPointF(sceneSize.x(), y)));
	}

	// command layer
	painter.setPen(Qt::gray);
	for (const auto& [_, item] : items)
	{
		if (item.endPos)
		{
			painter.drawPixmap(
				sceneTransform.map(*item.endPos + pixOffset),
				QPixmap(":/images/waypoint").scaled(sceneScale, sceneScale, Qt::AspectRatioMode::KeepAspectRatio));
		}
	}

	// draw visible range
	painter.setBrush(QBrush(QColor(0, 255, 0, 64)));
	for (const auto& [id, item] : items)
	{
		auto viewer = context->getComponent<game::ViewerComponent>(id);
		if (!viewer)
		{
			continue;
		}
		math::foreachCircle(
			math::Vector2{static_cast<int64_t>(item.pos.x()), static_cast<int64_t>(item.pos.y())},
			0,
			viewer->range,
			[this, &painter, item](const math::Vector2& point)
			{
				const QPointF topLeft = sceneTransform.map(toPointF(point.x, point.y));
				painter.drawRect(QRectF(topLeft, QSizeF{1.0 * sceneScale, 1.0 * sceneScale}));
				return true;
			});
	}

	// cropse layer
	for (const Item& item : died)
	{
		const QPointF pos = sceneTransform.map(item.pos + pixOffset);
		painter.drawPixmap(pos, styles[item.type].scaled(sceneScale, sceneScale, Qt::AspectRatioMode::KeepAspectRatio));
	}

	// unit layer
	painter.setPen(Qt::black);
	for (const auto& [_, item] : items)
	{
		const QPointF pos = sceneTransform.map(item.pos + pixOffset);
		painter.drawPixmap(pos, styles[item.type].scaled(sceneScale, sceneScale, Qt::AspectRatioMode::KeepAspectRatio));
	}

	// attacks layer
	for (const auto [dir, type] : attacks)
	{
		painter.setPen(type == 1 ? Qt::red : Qt::green);//1 - attack, 2 - heal
		painter.drawLine(
			sceneTransform.map(dir.p1() + QPointF(0.5, 0.5)), sceneTransform.map(dir.p2() + QPointF(0.5, 0.5)));
	}
}

void GameSceneWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	updateViewTransform();
}

void GameSceneWidget::clearTempLayer()
{
	attacks.clear();
	repaint();
}

void GameSceneWidget::updateViewTransform()
{
	if (width() <= 0 || height() <= 0 || sceneSize.x() <= 0 || sceneSize.y() <= 0)
	{
		return;
	}

	const QPointF widgetSize{static_cast<qreal>(width()), static_cast<qreal>(height())};

	const qreal extension = 2.0;
	const qreal scaleX = widgetSize.x() / (sceneSize.x() + extension);
	const qreal scaleY = widgetSize.y() / (sceneSize.y() + extension);
	sceneScale = std::min(scaleX, scaleY);

	const QPointF scaledSceneSize = sceneSize * sceneScale;
	const QPointF offset = (widgetSize - scaledSceneSize) / 2.0;

	sceneTransform = QTransform();
	sceneTransform.translate(offset.x(), offset.y());
	sceneTransform.scale(sceneScale, sceneScale);
}

void GameSceneWidget::setSceneSize(const QPointF& inSceneSize)
{
	sceneSize = inSceneSize;
	updateViewTransform();
	repaint();
}
