#include "UnitControlSystem.hpp"

#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/UnitComponent.hpp"
#include "Game/Components/WeaponComponent.hpp"

#include <ECS/Context.hpp>

namespace sw::game
{
	UnitControlSystem::UnitControlSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{}

	void UnitControlSystem::advance()
	{
		context->for_each<UnitComponent, MovementComponent>(
			[this](ecs::Entity& entity, auto unit, auto movement)
			{
				for (const auto priority : unit->priorityOrder)
				{
					switch (priority)
					{
						case BehaviourPriorityType::Attack:
							if (DoAttack(entity.id, unit->pos))
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

	bool UnitControlSystem::DoAttack(const uint32_t entityId, const math::Vector2u& pos)
	{
		auto weaponComponent = context->getComponent<WeaponComponent>(entityId);

		// find sutable weapon
		for (auto& [weaponId, weapon] : weaponComponent->weapons)
		{
			if (const uint32_t targetId = findTarget(entityId, pos, weapon); targetId != InvalidId)
			{
				weapon.targetId = targetId;
				return true;
			}
		}
		return false;
	}

	uint32_t UnitControlSystem::findTarget(
		const uint32_t selfId, const math::Vector2u& pos, const Weapon& weapon) const
	{
		uint32_t targetId = InvalidId;
		context->for_each<UnitComponent, DamageTakerComponent>(
			[selfId, &targetId, pos, weapon](ecs::Entity& entity, auto unit, auto damageTaker)
			{
				// target have health and can be damaged by with sort of weapon
				if (selfId == entity.id || damageTaker->health == 0 || !weapon.canDamage.contains(unit->type))
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
}
