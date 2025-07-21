#include "GameSceneWidget.hpp"

#include <ECS/Context.hpp>
#include <Game/Components/ViewerComponent.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
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
	return QSize(320, 240);
}

void GameSceneWidget::paintEvent(QPaintEvent* event)
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

	// command layer
	painter.setPen(Qt::NoPen);
	for (const auto& [_, item] : items)
	{
		if (item.endPos)
		{
			painter.setBrush(Qt::gray);
			painter.drawEllipse(
				sceneTransform.map(*item.endPos + QPointF(0.5, 0.5)), 0.4 * sceneScale, 0.4 * sceneScale);
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

	static std::unordered_map<std::string, QBrush> styles{
														  {"Swordsman", QBrush(Qt::red)}, {"Hunter", QBrush(Qt::green)}, {"died", QBrush(Qt::gray)}};

	// cropse layer
	for (const Item& item : died)
	{
		painter.setBrush(styles[item.type]);
		painter.drawEllipse(sceneTransform.map(item.pos + QPointF(0.5, 0.5)), 0.3 * sceneScale, 0.3 * sceneScale);
	}

	// unit layer
	painter.setPen(Qt::black);
	for (const auto& [_, item] : items)
	{
		painter.setBrush(styles[item.type]);
		painter.drawEllipse(sceneTransform.map(item.pos + QPointF(0.5, 0.5)), 0.3 * sceneScale, 0.3 * sceneScale);
	}
}

void GameSceneWidget::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	updateViewTransform();
}

void GameSceneWidget::updateViewTransform()
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

void GameSceneWidget::setSceneSize(const QPointF& inSceneSize)
{
	sceneSize = inSceneSize;
	updateViewTransform();
	repaint();
}
