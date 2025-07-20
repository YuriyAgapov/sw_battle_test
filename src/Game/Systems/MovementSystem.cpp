#include "MovementSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/GridComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Events/SetMovementBoundsEvent.hpp"

#include <ECS/Context.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::game
{
	MovementSystem::MovementSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<SetMovementBoundsEvent>(
			[this](const SetMovementBoundsEvent& event)
			{
				auto grid = this->context->getSingletoneComponent<GridComponent>();
				grid->bounds = event.bounds;
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

				EventLog::log(context->getTickCount(), io::UnitMoved{entityId, movement->pos.getX(), movement->pos.getY()});

				return true;
			});
	}
}
