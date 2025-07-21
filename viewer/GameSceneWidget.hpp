#pragma once

#include <QWidget>

namespace sw::ecs { class Context; }

class GameSceneWidget : public QWidget
{
public:
	GameSceneWidget(const std::shared_ptr<sw::ecs::Context>& context, QWidget* parent);

	virtual QSize sizeHint() final;
	virtual void paintEvent(QPaintEvent* event) final;
	virtual void resizeEvent(QResizeEvent* event) final;

private:
	void updateViewTransform();
	void setSceneSize(const QPointF& inSceneSize);

	std::shared_ptr<sw::ecs::Context> context;

	QPointF sceneSize;
	QTransform sceneTransform;
	qreal sceneScale = 1.0;

	struct Item
	{
		std::string type;
		QPointF pos;
		std::optional<QPointF> endPos;
	};

	std::unordered_map<uint32_t, Item> items;
	std::vector<Item> died;
};

