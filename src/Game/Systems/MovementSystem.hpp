#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/UnitComponent.hpp"
#include "Game/Events/MovementTargetEvent.hpp"
#include "Game/Events/SetMovementEvent.hpp"

namespace sw::game
{
	struct MovementSystem : public ecs::System
	{
	public:
		MovementSystem(const std::shared_ptr<ecs::Context>& context) :
				context(context)
		{
			context->getDispatcher().subscribe<SetMovementEvent>(
				[this](const SetMovementEvent& event)
				{
					auto movement = this->context->getComponent<MovementComponent>(event.causerId);
					movement->active = event.active;
				});
			context->getDispatcher().subscribe<MovementTargetEvent>(
				[this](const MovementTargetEvent& event)
				{
					auto movement = this->context->getComponent<MovementComponent>(event.causerId);
					movement->target = event.target;
				});
		}

		void advance() final
		{
			context->for_each<UnitComponent, MovementComponent>(
				[](auto entity, auto unit, auto movement)
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
