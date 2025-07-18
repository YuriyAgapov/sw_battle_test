#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/UnitComponent.hpp"
#include "Game/Events/SetMovementTargetEvent.hpp"

#include <IO/System/EventLog.hpp>

#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitMoved.hpp>

namespace sw::game
{
	struct MovementSystem : public ecs::System
	{
	public:
		MovementSystem(const std::shared_ptr<ecs::Context>& context) :
				context(context)
		{
			context->getDispatcher().subscribe<SetMovementTargetEvent>(
				[this](const SetMovementTargetEvent& event)
				{
					auto movement = this->context->getComponent<MovementComponent>(event.entityId);
					movement->target = event.target;

					EventLog::log(this->context->getTickCount(), io::MarchStarted{event.entityId, event.target.x, event.target.y});
				});
		}

		void advance() final
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

					unit->pos = math::moveTo(
									math::round<math::Vector2d>(unit->pos),
									math::round<math::Vector2d>(target),
									movement->speed)
									.template to<math::Vector2u>();

					EventLog::log(context->getTickCount(), io::UnitMoved{entity.id, unit->pos.x, unit->pos.y});

					// coplete way if arrived
					if (unit->pos == target)
					{
						movement->target.reset();
					}
					return true;
				});
		}

	private:
		std::shared_ptr<ecs::Context> context;
	};
}
