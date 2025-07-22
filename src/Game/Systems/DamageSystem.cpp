#include "DamageSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"

#include <ECS/Context.hpp>
#include <Game/Commands/DamageCommand.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitHealed.hpp>

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

		context->getDispatcher().subscribe<DamageCommand>([this](const DamageCommand& comamnd)
														  { performDamage(comamnd); });
	}

	void DamageSystem::advance()
	{
		// nothing
	}

	void DamageSystem::performDamage(const DamageCommand& command)
	{
		auto causerDamageTaker = context->getComponent<DamageTakerComponent>(command.causerId);
		// skip if causer died
		if (causerDamageTaker && causerDamageTaker->health == 0)
		{
			// interpret like the event was rolled back
			return;
		}

		auto targetDamageTaker = context->getComponent<DamageTakerComponent>(command.targetId);

		// skip if already died
		if (!targetDamageTaker || targetDamageTaker->health == 0)
		{
			//point for improvement: rules to revive with heal
			return;
		}

		// handle damage
		switch (command.damageType)
		{
			case DamageType::Regular:
			{
				const uint32_t damage = calcDamage(command);
				targetDamageTaker->health -= std::min(targetDamageTaker->health, damage);

				context->getDispatcher() << io::UnitAttacked{
															 command.causerId, command.targetId, damage, targetDamageTaker->health};

				//point for improvement: death/revive rules
				if (targetDamageTaker->health == 0)
				{
					context->removeEntity(command.targetId);
					context->getDispatcher() << io::UnitDied{command.targetId};
				}
			}
			break;

			case DamageType::Heal:
				{
					const uint32_t healAmount = calcHeal(command);
					targetDamageTaker->health
						= std::min(targetDamageTaker->health + calcHeal(command), targetDamageTaker->maxHealth);

					//todo: log heal
					context->getDispatcher() << io::UnitHealed{command.causerId, command.targetId, healAmount, targetDamageTaker->health};
				}
				break;
			default: break;
		}
	}

	uint32_t DamageSystem::calcDamage(const DamageCommand& command) const
	{
		// point for improvement: resist, crit, handling overdamage or other damage modifiers
		uint32_t change = 0;

		// reduce damage by the air untits from the ground based (raven case)
		if (command.weaponType == WeaponType::Range
			&& getUnitType(context, command.causerId) == DispositionType::Ground
			&& getUnitType(context, command.targetId) == DispositionType::Air)
		{
			change -= 1;
		}
		return command.amount + change;
	}

	uint32_t DamageSystem::calcHeal(const DamageCommand& command) const
	{
		return command.amount;
	}
}
