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
		UnitMovementSystem(std::shared_ptr<ecs::Context> context) {}

		void advance()
		{
			context->for_each<UnitComponent, UnitMovementComponent>(
				[](auto UnitComponent, auto MovementComponent)
				{
					if (UnitComponent->actionPoints < 1 || !MovementComponent->target || MovementComponent->speed == 0)
					{
						return;
					}

					const math::Vector2u target = *MovementComponent->target;
					UnitComponent->pos = math::moveTo(UnitComponent->pos, target, MovementComponent->speed);
					if (UnitComponent->pos == target)
					{
						MovementComponent->target.reset();
					}
				});
		}

	private:
		std::shared_ptr<ecs::Context> context;
	};
}
