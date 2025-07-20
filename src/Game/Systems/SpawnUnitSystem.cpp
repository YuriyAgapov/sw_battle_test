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
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Events/UnitSpawned.hpp>

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

		auto grid = context->getSingletoneComponent<game::GridComponent>();
		grid->mapping.add(data.unitId, pos);

		return data.unitId;
	}

	SpawnUnitSystem::SpawnUnitSystem(const std::shared_ptr<ecs::Context>& context) :
			System(context)
	{
		debug::check(context, "invalid context");

		context->getDispatcher().subscribe<io::SpawnSwordsman>(
			[context](const io::SpawnSwordsman& event)
			{
				auto swordsman = createBaseUnit(context, event);
				auto weaponry = context->addComponent<game::WeaponComponent>(swordsman);
				weaponry->weapons = game::WeaponMap{
													{Weapon::swordId,
					 game::Weapon{
								  event.strength,
						 0,	 //min
						 1,	 //max
						 game::DamageType::Regular,
						 game::WeaponType::Melee,
						 std::unordered_set<game::DispositionType>{game::DispositionType::Ground}}}};

				auto viewer = context->addComponent<game::ViewerComponent>(swordsman);
				viewer->range = 1;

				context->getDispatcher() << io::UnitSpawned{event.unitId, "Swordsman", event.x, event.y};
			});
		context->getDispatcher().subscribe<io::SpawnHunter>(
			[context](const io::SpawnHunter& event)
			{
				auto hunter = createBaseUnit(context, event);
				auto weaponry = context->addComponent<game::WeaponComponent>(hunter);
				weaponry->weapons = game::WeaponMap{
													{Weapon::swordId,
					 game::Weapon{
								  event.strength,
						 0,	 //min
						 1,	 //max
						 game::DamageType::Regular,
						 game::WeaponType::Melee,
						 std::unordered_set<game::DispositionType>{game::DispositionType::Ground}}},
					{Weapon::bowId,
					 game::Weapon{
								  event.agility,
						 2,	 //min
						 event.range,
						 game::DamageType::Regular,
						 game::WeaponType::Range,
						 std::unordered_set<game::DispositionType>{
																   game::DispositionType::Ground, game::DispositionType::Air}}}};

				auto viewer = context->addComponent<game::ViewerComponent>(hunter);
				viewer->range = event.range;

				context->getDispatcher() << io::UnitSpawned{event.unitId, "Hunter", event.x, event.y};
			});
	}

	void SpawnUnitSystem::advance()
	{
		// nothing
	}
}
