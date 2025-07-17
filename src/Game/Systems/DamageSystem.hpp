#pragma once

#include "ECS/Context.hpp"
#include "ECS/System.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Events/DamageEvent.hpp"

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
							damageTaker->health = std::max(damageTaker->health - calcDamage(damageEvent), 0u);
							break;

						case DamageType::Heal:
							damageTaker->health
								= std::min(damageTaker->health + calcHeal(damageEvent), damageTaker->maxHealth);
							break;
						default: break;
					}
				});
		}

		void advance() final
		{
			context->for_each<DamageTakerComponent>(
				[this](auto entity, auto damageTaker)
				{
					if (damageTaker->health == 0)
					{
						//point for improvement: death/revive rules
						entity.deleteLater = true;
					}
					return true;
				});
		}

	private:
		uint32_t calcDamage(const DamageEvent& damageEvent) const
		{
			// point for improvement: resist, crit, handling overdamage or other damage modifiers
			return damageEvent.amount;
		}

		uint32_t calcHeal(const DamageEvent& damageEvent) const
		{
			return damageEvent.amount;
		}

		std::shared_ptr<ecs::Context> context;
	};
}
