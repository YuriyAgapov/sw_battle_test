#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/UnitComponent.hpp"
#include "Game/Events/DamageEvent.hpp"
#include "Game/Events/DeathEvent.hpp"

#include <IO/System/EventLog.hpp>

#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>

namespace sw::game
{
	struct DamageSystem : public ecs::System
	{
	public:
		DamageSystem(const std::shared_ptr<ecs::Context>& context) :
				context(context)
		{
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
					switch (damageEvent.type)
					{
						case DamageType::Melee:
						case DamageType::Range:
							{
								const uint32_t damage = calcDamage(damageEvent);
								damageTaker->health -= std::min(damageTaker->health, damage);

								EventLog::log(this->context->getTickCount(), io::UnitAttacked{damageEvent.causerId, damageEvent.targetId, damage, damageTaker->health});
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

		void advance() final
		{
			context->for_each<DamageTakerComponent>(
				[this](ecs::Entity& entity, auto damageTaker)
				{
					if (damageTaker->health == 0)
					{
						//point for improvement: death/revive rules
						entity.deleteLater = true;

						context->getDispatcher() << DeathEvent{entity.id};

						EventLog::log(context->getTickCount(), io::UnitDied{entity.id});
					}
					return true;
				});
		}

	private:
		uint32_t calcDamage(const DamageEvent& damageEvent) const
		{
			// point for improvement: resist, crit, handling overdamage or other damage modifiers
			uint32_t change = 0;

			// raven case
			if (damageEvent.type == DamageType::Range && getUnitType(damageEvent.causerId) == UnitType::Ground && getUnitType(damageEvent.targetId) == UnitType::Flying)
			{
				change += 1;
			}
			return damageEvent.amount + change;
		}

		uint32_t calcHeal(const DamageEvent& damageEvent) const
		{
			return damageEvent.amount;
		}

		UnitType getUnitType(const uint32_t entityId) const
		{
			auto unit = context->getComponent<UnitComponent>(entityId);
			return unit ? unit->type : UnitType::Ground;
		}

		std::shared_ptr<ecs::Context> context;
	};
}
