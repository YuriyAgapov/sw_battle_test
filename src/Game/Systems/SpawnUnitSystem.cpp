#include "SpawnUnitSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/BehaviourComponent.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/ViewerComponent.hpp"
#include "Game/Components/VisibleComponent.hpp"
#include "Game/Components/WeaponComponent.hpp"

#include <ECS/Context.hpp>
#include <Game/Components/GridComponent.hpp>
#include <IO/Commands/SpawnHealer.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Commands/SpawnTower.hpp>
#include <IO/Events/UnitSpawned.hpp>

namespace sw::game
{
	template <typename TCommand>
	uint32_t getSpeed(const TCommand&)
	{
		return 1;
	}

	template <>
	uint32_t getSpeed<io::SpawnTower>(const io::SpawnTower&)
	{
		return 0;
	}

	template <typename TCommand>
	uint32_t getRangeDamage(const TCommand& command)
	{
		if constexpr (requires { command.power; })
		{
			return command.power;
		}
		if constexpr (requires { command.agility; })
		{
			return command.agility;
		}
		return 0;
	}

	template <typename TCommand>
	math::Vector2 getWeaponRange(const TCommand& command)
	{
		return {0, 1};
	}

	template <>
	math::Vector2 getWeaponRange(const io::SpawnHunter& command)
	{
		return {2, command.range};
	}

	template <>
	math::Vector2 getWeaponRange(const io::SpawnTower& command)
	{
		return {2, 5};
	}

	template <>
	math::Vector2 getWeaponRange(const io::SpawnHealer& command)
	{
		return {0, 2};
	}

	template <typename TCommand>
	inline uint32_t createUnit(
		const std::shared_ptr<ecs::Context>& context, const TCommand& command, const uint32_t priority)
	{
		debug::check(context->addEntity(command.unitId), "can't create entity");

		context->addComponent<game::VisibleComponent>(command.unitId);

		auto behaviour = context->addComponent<game::BehaviourComponent>(command.unitId);
		behaviour->priority = priority;

		if constexpr (requires {
						  command.x;
						  command.y;
					  })
		{
			const math::Vector2 pos{command.x, command.y};
			debug::checkPosition(context, pos);
			auto movement = context->addComponent<game::MovementComponent>(command.unitId);
			movement->pos = pos;
			movement->type = game::DispositionType::Ground;
			movement->speed = getSpeed(command);
		}

		if constexpr (requires { command.hp; })
		{
			auto damageTaker = context->addComponent<game::DamageTakerComponent>(command.unitId);
			damageTaker->health = command.hp;
			damageTaker->maxHealth = command.hp;
		}

		const math::Vector2 weaponRange = getWeaponRange(command);
		auto viewer = context->addComponent<game::ViewerComponent>(command.unitId);
		viewer->range = weaponRange.getY();

		if (const uint32_t rangeDamage = getRangeDamage(command); rangeDamage > 0)
		{
			auto weaponry = context->addComponent<game::WeaponComponent>(command.unitId);
			weaponry->weapons.emplace_back(game::Weapon{
				rangeDamage,
				weaponRange.getX(),
				weaponRange.getY(),
				game::DamageType::Regular,
				game::WeaponType::Range,
				std::unordered_set<game::DispositionType>{game::DispositionType::Ground, game::DispositionType::Air}});
		}

		if constexpr (requires { command.strength; })
		{
			auto weaponry = context->ensureComponent<game::WeaponComponent>(command.unitId);
			weaponry->weapons.emplace_back(game::Weapon{
				command.strength,
				0,
				1,
				game::DamageType::Regular,
				game::WeaponType::Melee,
				std::unordered_set<game::DispositionType>{game::DispositionType::Ground}});
		}

		if constexpr (requires { command.spirit; })
		{
			auto weaponry = context->ensureComponent<game::WeaponComponent>(command.unitId);
			weaponry->weapons.emplace_back(game::Weapon{
				command.spirit,
				0,
				weaponRange.getY(),
				game::DamageType::Heal,
				game::WeaponType::Range,
				std::unordered_set<game::DispositionType>{game::DispositionType::Ground}});
		}

		return command.unitId;
	}

	SpawnUnitSystem::SpawnUnitSystem(const std::shared_ptr<ecs::Context>& inContext) :
			System(inContext)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<io::SpawnSwordsman>(
			[this](const io::SpawnSwordsman& command)
			{
				createUnit(context, command, ++globalOrder);

				context->getDispatcher() << io::UnitSpawned{command.unitId, "Swordsman", command.x, command.y};
			});
		context->getDispatcher().subscribe<io::SpawnHunter>(
			[this](const io::SpawnHunter& command)
			{
				createUnit(context, command, ++globalOrder);
				context->getDispatcher() << io::UnitSpawned{command.unitId, "Hunter", command.x, command.y};
			});
		context->getDispatcher().subscribe<io::SpawnTower>(
			[this](const io::SpawnTower& command)
			{
				createUnit(context, command, ++globalOrder);
				context->getDispatcher() << io::UnitSpawned{command.unitId, "Tower", command.x, command.y};
			});
		context->getDispatcher().subscribe<io::SpawnHealer>(
			[this](const io::SpawnHealer& command)
			{
				createUnit(context, command, ++globalOrder);
				context->getDispatcher() << io::UnitSpawned{command.unitId, "Healer", command.x, command.y};
			});
	}

	void SpawnUnitSystem::advance()
	{
		// nothing
	}
}
