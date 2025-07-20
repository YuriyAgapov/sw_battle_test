#include "DamageSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Events/DamageEvent.hpp"
#include "Game/Events/DeathEvent.hpp"

#include <ECS/Context.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::game
{
	static DispositionType getUnitType(const std::shared_ptr<ecs::Context>& context, const uint32_t entityId)
	{
		assert(context);

		auto movement = context->getComponent<MovementComponent>(entityId);
		return movement ? movement->type : DispositionType::Ground;
	}

	DamageSystem::DamageSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<DamageEvent>(
			[this](const DamageEvent& damageEvent)
			{
				auto damageTaker = this->context->getComponent<DamageTakerComponent>(damageEvent.targetId);

				// skip if already died
				if (damageTaker->health == 0)
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
						damageTaker->health -= std::min(damageTaker->health, damage);

						EventLog::log(
							this->context->getTickCount(),
							io::UnitAttacked{damageEvent.causerId, damageEvent.targetId, damage, damageTaker->health});
					}
					break;

					case DamageType::Heal:
						damageTaker->health
							= std::min(damageTaker->health + calcHeal(damageEvent), damageTaker->maxHealth);

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
				if (damageTaker->health == 0)
				{
					//point for improvement: death/revive rules
					context->removeEntity(entityId);

					context->getDispatcher() << DeathEvent{entityId};

					EventLog::log(context->getTickCount(), io::UnitDied{entityId});
				}
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
