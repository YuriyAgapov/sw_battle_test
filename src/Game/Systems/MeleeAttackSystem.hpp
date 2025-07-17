#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/MeleeAttackComponent.hpp"
#include "Game/Components/UnitComponent.hpp"

#include <Game/Components/DamageTakerComponent.hpp>
#include <algorithm>

namespace sw::game
{
	struct MeleeAttackSystem : public ecs::System
	{
	public:
		MeleeAttackSystem(std::shared_ptr<ecs::Context> context) :
				context(context)
		{}

		void advance() final
		{
			context->for_each<UnitComponent, MeleeAttackComponent>(
				[this](std::shared_ptr<UnitComponent> unit, std::shared_ptr<MeleeAttackComponent> melee)
				{
					if (unit->actionPoints < 1)
					{
						return true;
					}
					if (auto target = findTarget(unit->pos, melee->range + 0.5))
					{
						//TODO point to introduce the damage pipeline
						target->health = std::max(target->health - melee->strength, 0u);
					}
					return true;
				});
		}

		std::shared_ptr<DamageTakerComponent> findTarget(const math::Vector2u& pos, const double range)
		{
			std::shared_ptr<DamageTakerComponent> target;
			context->for_each<UnitComponent, DamageTakerComponent>(
				[&target, pos, range](auto unit, auto damageTaker)
				{
					if (damageTaker->health == 0 || math::length(unit->pos - pos) > range)
					{
						return true;
					}
					target = damageTaker;
					return false;
				});
			return target;
		}

	private:
		std::shared_ptr<ecs::Context> context;
	};
}
