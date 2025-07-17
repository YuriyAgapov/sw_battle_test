#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/UnitComponent.hpp"
#include "Game/Components/UnitMovementComponent.hpp"

namespace sw::game
{
	struct UnitMovementSystem : public ecs::System
	{
	public:
		UnitMovementSystem(std::shared_ptr<ecs::Context> context) :
				context(context)
		{}

		void advance()
		{
			context->for_each<UnitComponent, UnitMovementComponent>(
				[](auto unit, auto movement)
				{
					if (unit->actionPoints < 1 || !movement->target || movement->speed == 0)
					{
						return true;
					}

					const math::Vector2u target = *movement->target;
					unit->pos = math::moveTo(unit->pos, target, movement->speed);
					--unit->actionPoints;
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
