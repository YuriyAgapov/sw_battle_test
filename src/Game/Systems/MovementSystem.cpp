#include "MovementSystem.hpp"

#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/UnitComponent.hpp"
#include "Game/Events/SetMovementTargetEvent.hpp"

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
		context->getDispatcher().subscribe<SetMovementTargetEvent>(
			[this](const SetMovementTargetEvent& event)
			{
				auto movement = this->context->getComponent<MovementComponent>(event.entityId);
				movement->target = event.target;

				EventLog::log(
					this->context->getTickCount(), io::MarchStarted{event.entityId, event.target.x, event.target.y});
			});
	}

	void MovementSystem::advance()
	{
		context->for_each<UnitComponent, MovementComponent>(
			[this](ecs::Entity& entity, auto unit, auto movement)
			{
				if (!movement->canMove())
				{
					return true;
				}

				// move by
				const math::Vector2u target = *movement->target;

				unit->pos
					= math::moveTo(
						  math::round<math::Vector2d>(unit->pos), math::round<math::Vector2d>(target), movement->speed)
						  .template to<math::Vector2u>();

				// coplete way if arrived
				if (unit->pos == target)
				{
					EventLog::log(context->getTickCount(), io::MarchEnded{entity.id, unit->pos.x, unit->pos.y});

					movement->target.reset();
				}
				else
				{
					EventLog::log(context->getTickCount(), io::UnitMoved{entity.id, unit->pos.x, unit->pos.y});
				}
				return true;
			});
	}
}
