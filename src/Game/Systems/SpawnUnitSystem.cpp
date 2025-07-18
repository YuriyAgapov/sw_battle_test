#include "SpawnUnitSystem.hpp"

#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/UnitComponent.hpp"
#include "Game/Components/WeaponComponent.hpp"
#include "Game/Events/SpawnUnitEvent.hpp"

#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::game
{
	template <typename TData>
	inline ecs::Entity& createBaseUnit(const std::shared_ptr<ecs::Context>& context, const TData& data)
	{
		auto& entity = context->addEntity<MovementComponent>(data.unitId);
		auto unit = context->addComponent<game::UnitComponent>(entity);
		unit->type = game::UnitType::Ground;
		unit->pos = math::Vector2u{data.x, data.y};
		auto damageTaker = context->addComponent<game::DamageTakerComponent>(entity);
		damageTaker->health = data.hp;
		damageTaker->maxHealth = data.hp;

		return entity;
	}

	SpawnUnitSystem::SpawnUnitSystem(const std::shared_ptr<ecs::Context>& context) :
			context(context)
	{
		context->getDispatcher().subscribe<SpawnSwordsmanUnitEvent>(
			[context](const SpawnSwordsmanUnitEvent& event)
			{
				auto swordsman = createBaseUnit(context, event.data);
				auto weaponry = context->addComponent<game::WeaponComponent>(swordsman);
				weaponry->weapons = game::WeaponMap{
													{1u,
					 game::Weapon{
								  event.data.strength,
						 0,
						 1,
						 game::DamageType::Melee,
						 std::unordered_set<game::UnitType>{game::UnitType::Ground}}}};

				EventLog::log(
					context->getTickCount(),
					io::UnitSpawned{event.data.unitId, "Swordsman", event.data.x, event.data.y});
			});
		context->getDispatcher().subscribe<SpawnHunterUnitEvent>(
			[context](const SpawnHunterUnitEvent& event)
			{
				auto hunter = createBaseUnit(context, event.data);
				auto weaponry = context->addComponent<game::WeaponComponent>(hunter);
				weaponry->weapons = game::WeaponMap{
													{1u,
					 game::Weapon{
								  event.data.strength,
						 0,
						 1,
						 game::DamageType::Melee,
						 std::unordered_set<game::UnitType>{game::UnitType::Ground}}},
					{2u,
					 game::Weapon{
								  event.data.agility,
						 2,
						 event.data.range,
						 game::DamageType::Range,
						 std::unordered_set<game::UnitType>{game::UnitType::Ground, game::UnitType::Flying}}}};

				EventLog::log(
					context->getTickCount(), io::UnitSpawned{event.data.unitId, "Hunter", event.data.x, event.data.y});
			});
	}

	void SpawnUnitSystem::advance()
	{
		// nothing
	}
}
