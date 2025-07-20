#include "MovementSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/GridComponent.hpp"
#include "Game/Components/MovementComponent.hpp"

#include <ECS/Context.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/UnitMoved.hpp>

namespace sw::game
{
	MovementSystem::MovementSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<io::CreateMap>(
			[context](const io::CreateMap& event)
			{
				auto grid = context->getSingletoneComponent<GridComponent>();
				grid->bounds = math::Rect2(event.width, event.height);
				if (!grid->bounds.isValid())
					throw std::runtime_error("Failed to CreateMap, bounds is invalid");

				context->getDispatcher() << io::MapCreated{event.width, event.height};
			});
	}

	void MovementSystem::advance()
	{
		context->for_each<MovementComponent>(
			[this, grid = context->getSingletoneComponent<GridComponent>()](const uint32_t entityId, auto movement)
			{
				if (movement->velocity.isZero())
				{
					return true;
				}

				grid->mapping.remove(entityId, movement->pos);
				movement->pos = movement->pos + movement->velocity;
				grid->mapping.add(entityId, movement->pos);

				context->getDispatcher() << io::UnitMoved{entityId, movement->pos.getX(), movement->pos.getY()};

				return true;
			});
	}
}
