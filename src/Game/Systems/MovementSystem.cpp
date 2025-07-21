#include "MovementSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/BehaviourComponent.hpp"
#include "Game/Components/GridComponent.hpp"
#include "Game/Components/MovementComponent.hpp"

#include <ECS/Context.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>

namespace sw::game
{
	MovementSystem::MovementSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<io::CreateMap>(
			[context](const io::CreateMap& event)
			{
				auto grid = context->addSingletoneComponent<game::GridComponent>();
				grid->bounds = math::Rect2(event.width, event.height);
				if (!grid->bounds.isValid())
					throw std::runtime_error("Failed to CreateMap, bounds is invalid");

				context->getDispatcher() << io::MapCreated{event.width, event.height};
			});
		context->getDispatcher().subscribe<io::UnitSpawned>(
			[context](const io::UnitSpawned& event)
			{
				auto movement = context->getComponent<MovementComponent>(event.unitId);
				auto grid = context->getSingletoneComponent<game::GridComponent>();
				grid->mapping.add(event.unitId, movement->pos);
			});
		context->getDispatcher().subscribe<io::UnitDied>(
			[context](const io::UnitDied& event)
			{
				auto movement = context->getComponent<MovementComponent>(event.unitId);
				auto grid = context->getSingletoneComponent<game::GridComponent>();
				grid->mapping.remove(event.unitId, movement->pos);
			});
	}

	void MovementSystem::advance()
	{
		auto view = context->makeView<BehaviourComponent, MovementComponent>();
		std::sort(
			view.begin(),
			view.end(),
			[](const auto& left, const auto& right)
			{
				return std::get<1>(left)->priority < std::get<1>(right)->priority;
			});

		auto grid = context->getSingletoneComponent<GridComponent>();
		for (const auto& [entityId, behaviour, movement] : view)
		{
			if (movement->velocity.isZero())
			{
				continue;
			}

			grid->mapping.remove(entityId, movement->pos);
			movement->pos = movement->pos + movement->velocity;
			grid->mapping.add(entityId, movement->pos);

			context->getDispatcher() << io::UnitMoved{entityId, movement->pos.getX(), movement->pos.getY()};
		}
	}
}
