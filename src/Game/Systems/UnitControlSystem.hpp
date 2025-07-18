#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/UnitComponent.hpp"

#include <Game/Components/WeaponComponent.hpp>

namespace sw::game
{
	class UnitControlSystem : public ecs::System
	{
	public:
		UnitControlSystem(const std::shared_ptr<ecs::Context>& context) :
				context(context)
		{}

		void advance() final
		{
			context->for_each<UnitComponent, MovementComponent>(
				[this](ecs::Entity& entity, auto unit, auto movement)
				{
					for (const auto priority : unit->priorityOrder)
					{
						switch (priority)
						{
							case BehaviourPriorityType::Attack:
								if (DoAttack(entity, unit->pos))
								{
									movement->active = false;
									return true;
								}
							case BehaviourPriorityType::Movement:
								if (movement->target)
								{
									movement->active = true;
									return true;
								}
								break;
						}
					}
					return true;
				});
		}

	private:
		bool DoAttack(ecs::Entity& entity, const math::Vector2u& pos)
		{
			auto weaponComponent = context->getComponent<WeaponComponent>(entity.id);

			// find sutable weapon
			for (auto& [weaponId, weapon] : weaponComponent->weapons)
			{
				if (const uint32_t targetId = findTarget(entity, pos, weapon); targetId != InvalidId)
				{
					weapon.targetId = targetId;
					return true;
				}
			}
			return false;
		}

		inline uint32_t findTarget(const ecs::Entity& self, const math::Vector2u& pos, const Weapon& weapon) const
		{
			uint32_t targetId = InvalidId;
			context->for_each<UnitComponent, DamageTakerComponent>(
				[self, &targetId, pos, weapon](ecs::Entity& entity, auto unit, auto damageTaker)
				{
					// target have health and can be damaged by with sort of weapon
					if (self.id == entity.id || damageTaker->health == 0 || !weapon.canDamage.contains(unit->type))
					{
						return true;
					}
					const double range = math::length(unit->pos - pos);
					if (range < weapon.minRange || range > weapon.maxRange)
					{
						return true;
					}
					targetId = entity.id;
					return false;
				});
			return targetId;
		}

		std::shared_ptr<ecs::Context> context;
	};
}
