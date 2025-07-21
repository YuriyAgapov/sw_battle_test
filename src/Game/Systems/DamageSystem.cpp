#include "DamageSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Events/DamageEvent.hpp"

#include <ECS/Context.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>

namespace sw::game
{
	static DispositionType getUnitType(const std::shared_ptr<ecs::Context>& context, const uint32_t entityId)
	{
		assert(context);

		auto movement = context->getComponent<MovementComponent>(entityId);
		return movement ? movement->type : DispositionType::Ground;
	}

	DamageSystem::DamageSystem(const std::shared_ptr<ecs::Context>& inContext) :
			System(inContext)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<DamageEvent>(
			[this](const DamageEvent& damageEvent)
			{
				auto causerDamageTaker = context->getComponent<DamageTakerComponent>(damageEvent.causerId);
				// skip if causer died
				if (causerDamageTaker->health == 0)
				{
					// interpret like the event was rolled back
					return;
				}

				auto targetDamageTaker = context->getComponent<DamageTakerComponent>(damageEvent.targetId);

				// skip if already died
				if (targetDamageTaker->health == 0)
				{
					//point for improvement: rules to revive with heal
					return;
				}

				// handle damage
				switch (damageEvent.damageType)
				{
					case DamageType::Regular:
					{
						const uint32_t damage = calcDamage(damageEvent);
						targetDamageTaker->health -= std::min(targetDamageTaker->health, damage);

						context->getDispatcher() << io::UnitAttacked{damageEvent.causerId, damageEvent.targetId, damage, targetDamageTaker->health};

						//point for improvement: death/revive rules
						if (targetDamageTaker->health == 0)
						{
							context->removeEntity(damageEvent.targetId);
							context->getDispatcher() << io::UnitDied{damageEvent.targetId};
						}
					}
					break;

					case DamageType::Heal:
						targetDamageTaker->health
							= std::min(targetDamageTaker->health + calcHeal(damageEvent), targetDamageTaker->maxHealth);

						//todo: log heal
						break;
					default: break;
				}
			});
	}

	void DamageSystem::advance()
	{
		context->for_each<DamageTakerComponent>(
			[this](const uint32_t entityId, auto damageTaker)
			{
				return true;
			});
	}

	uint32_t DamageSystem::calcDamage(const DamageEvent& damageEvent) const
	{
		// point for improvement: resist, crit, handling overdamage or other damage modifiers
		uint32_t change = 0;

		// reduce damage by the air untits from the ground based (raven case)
		if (damageEvent.weaponType == WeaponType::Range
			&& getUnitType(context, damageEvent.causerId) == DispositionType::Ground
			&& getUnitType(context, damageEvent.targetId) == DispositionType::Air)
		{
			change -= 1;
		}
		return damageEvent.amount + change;
	}

	uint32_t DamageSystem::calcHeal(const DamageEvent& damageEvent) const
	{
		return damageEvent.amount;
	}
}
