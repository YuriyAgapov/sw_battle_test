#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/UnitComponent.hpp"
#include "Game/Events/SetMovementActiveEvent.hpp"
#include "Game/Events/ActivateWeaponEvent.hpp"

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
				[this](auto entity, auto unit, auto movement)
				{
					for (const auto priority : unit->priorityOrder)
					{
						switch (priority)
						{
							case BehaviourPriorityType::Attack:
								if (DoAttack(entity, unit->pos))
								{
									context->getDispatcher() << SetMovementActiveEvent{entity.id, false};
									return true;
								}
							case BehaviourPriorityType::Movement:
								if (movement->target)
								{
									context->getDispatcher() << SetMovementActiveEvent{entity.id, true};
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
			for (const auto& [weaponId, weapon] : weaponComponent->weapons)
			{
				if (const uint32_t targetId = findTarget(pos, weapon); targetId != InvalidId)
				{
					// use weapon and return
					context->getDispatcher() << ActivateWeaponEvent{entity.id, targetId, weaponId};
					return true;
				}
			}
			return false;
		}

		inline uint32_t findTarget(const math::Vector2u& pos, const Weapon& weapon) const
		{
			uint32_t targetId = InvalidId;
			context->for_each<UnitComponent, DamageTakerComponent>(
				[&targetId, pos, weapon](auto entity, auto unit, auto damageTaker)
				{
					// target have health and can be damaged by with sort of weapon
					if (damageTaker->health == 0 || !weapon.canDamage.contains(unit->type))
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
