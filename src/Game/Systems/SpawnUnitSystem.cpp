#include "SpawnUnitSystem.hpp"

#include "Debug.hpp"
#include "Game/Components/BehaviourComponent.hpp"
#include "Game/Components/DamageTakerComponent.hpp"
#include "Game/Components/MovementComponent.hpp"
#include "Game/Components/ViewerComponent.hpp"
#include "Game/Components/VisibleComponent.hpp"
#include "Game/Components/WeaponComponent.hpp"
#include "Game/Events/SpawnUnitEvent.hpp"

#include <ECS/Context.hpp>
#include <Game/Components/GridComponent.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/EventLog.hpp>

namespace sw::game
{
	template <typename TData>
	inline uint32_t createBaseUnit(const std::shared_ptr<ecs::Context>& context, const TData& data)
	{
		const math::Vector2 pos{data.x, data.y};
		debug::checkPosition(context, pos);

		debug::check(context->addEntity(data.unitId), "can't create entity");

		context->addComponent<game::VisibleComponent>(data.unitId);

		auto movement = context->addComponent<game::MovementComponent>(data.unitId);
		movement->pos = pos;
		movement->type = game::DispositionType::Ground;

		auto unit = context->addComponent<game::BehaviourComponent>(data.unitId);

		auto damageTaker = context->addComponent<game::DamageTakerComponent>(data.unitId);
		damageTaker->health = data.hp;
		damageTaker->maxHealth = data.hp;

		return data.unitId;
	}

	SpawnUnitSystem::SpawnUnitSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<SpawnSwordsmanUnitEvent>(
			[context](const SpawnSwordsmanUnitEvent& event)
			{
				auto swordsman = createBaseUnit(context, event.data);
				auto weaponry = context->addComponent<game::WeaponComponent>(swordsman);
				weaponry->weapons = game::WeaponMap{
													{Weapon::swordId,
					 game::Weapon{
								  event.data.strength,
						 0,//min
						 1,//max
						 game::DamageType::Regular,
						 game::WeaponType::Melee,
						 std::unordered_set<game::DispositionType>{game::DispositionType::Ground}}}};

				auto viewer = context->addComponent<game::ViewerComponent>(swordsman);
				viewer->range = 1;

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
													{Weapon::swordId,
					 game::Weapon{
								  event.data.strength,
						 0,//min
						 1,//max
						 game::DamageType::Regular,
						 game::WeaponType::Melee,
						 std::unordered_set<game::DispositionType>{game::DispositionType::Ground}}},
					{Weapon::bowId,
					 game::Weapon{
								  event.data.agility,
						 2,//min
						 event.data.range,
						 game::DamageType::Regular,
						 game::WeaponType::Range,
						 std::unordered_set<game::DispositionType>{
																   game::DispositionType::Ground, game::DispositionType::Air}}}};

				auto viewer = context->addComponent<game::ViewerComponent>(hunter);
				viewer->range = event.data.range;

				EventLog::log(
					context->getTickCount(), io::UnitSpawned{event.data.unitId, "Hunter", event.data.x, event.data.y});
			});
	}

	void SpawnUnitSystem::advance()
	{
		// nothing
	}
}
