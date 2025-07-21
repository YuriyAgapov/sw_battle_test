#include "game_test_fixture.hpp"

#include <ECS/Context.hpp>
#include <ECS/System.hpp>
#include <Game/Components/BehaviourComponent.hpp>
#include <Game/Components/DamageTakerComponent.hpp>
#include <Game/Components/GridComponent.hpp>
#include <Game/Components/MovementComponent.hpp>
#include <Game/Components/ViewerComponent.hpp>
#include <Game/Components/VisibleComponent.hpp>
#include <Game/Components/WeaponComponent.hpp>
#include <Game/GameApp.hpp>
#include <Game/Systems/AISystem.hpp>
#include <Game/Systems/DamageSystem.hpp>
#include <Game/Systems/MovementSystem.hpp>
#include <Game/Systems/SpawnUnitSystem.hpp>
#include <Game/Systems/VisibilitySystem.hpp>
#include <Game/Systems/WeaponSystem.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <gtest/gtest.h>

using namespace sw;
using namespace sw::ecs;
using namespace sw::game;

TEST_F(GameTest, spawnSwordsman)
{
	const uint32_t unitId = 1;
	const uint32_t hp = 2;
	const uint32_t damage = 3;
	context->getDispatcher() << io::SpawnSwordsman{unitId, 5, 5, hp, damage};
	context->advance();

	auto visible = context->getComponent<VisibleComponent>(unitId);
	EXPECT_TRUE(visible);

	auto viewer = context->getComponent<ViewerComponent>(unitId);
	EXPECT_TRUE(viewer);
	EXPECT_EQ(viewer->range, 1);

	auto behaviour = context->getComponent<BehaviourComponent>(unitId);
	EXPECT_TRUE(behaviour);

	auto damageTaker = context->getComponent<DamageTakerComponent>(unitId);
	EXPECT_TRUE(damageTaker);
	EXPECT_EQ(damageTaker->health, hp);

	auto movement = context->getComponent<MovementComponent>(unitId);
	EXPECT_TRUE(movement);
	EXPECT_EQ(movement->pos.x, 5);
	EXPECT_EQ(movement->pos.y, 5);

	auto weapon = context->getComponent<WeaponComponent>(unitId);
	EXPECT_TRUE(weapon);
	EXPECT_EQ(weapon->weapons.size(), 1);
	EXPECT_EQ(weapon->weapons[0].damage, damage);
	EXPECT_EQ(weapon->weapons[0].minRange, 0);
	EXPECT_EQ(weapon->weapons[0].maxRange, 1);
	EXPECT_EQ(weapon->weapons[0].weaponType, WeaponType::Melee);
	EXPECT_EQ(weapon->weapons[0].damageType, DamageType::Regular);
	EXPECT_EQ(weapon->weapons[0].canDamage, std::unordered_set<DispositionType>{DispositionType::Ground});
}

TEST_F(GameTest, movementSystem)
{
	const uint32_t unitId = 1;
	const uint32_t hp = 2;
	const uint32_t damage = 3;
	context->getDispatcher() << io::SpawnSwordsman{unitId, 0, 0, hp, damage};
	context->getDispatcher() << io::March{unitId, 5, 2};
	context->getDispatcher().dispatchAll();

	auto movement = context->getComponent<MovementComponent>(unitId);

	std::vector<math::Vector2> actualPath;
	actualPath.reserve(6);
	for (uint8_t i = 0; i < 6; ++i)
	{
		actualPath.emplace_back(movement->pos);
		context->advance();
	}
	const std::vector<math::Vector2> expectedPath = {{0, 0}, {1, 0}, {2, 0}, {3, 1}, {4, 1}, {5, 2}};
	EXPECT_EQ(actualPath, expectedPath);
}

TEST_F(GameTest, visibilitySystem)
{
	const uint32_t unitA = 1;
	const uint32_t unitB = 2;
	const uint32_t hp = 999;
	const uint32_t damage = 1;
	context->getDispatcher() << io::SpawnSwordsman{unitA, 1, 1, hp, damage};
	context->getDispatcher() << io::SpawnSwordsman{unitB, 1, 2, hp, damage};
	context->getDispatcher().dispatchAll();
	context->advance();

	auto viewerA = context->getComponent<ViewerComponent>(unitA);
	auto viewerB = context->getComponent<ViewerComponent>(unitB);

	EXPECT_EQ(viewerA->visibleMapping.get(math::Vector2{1, 2}), (std::unordered_set<uint32_t>{unitB}));
	EXPECT_EQ(viewerB->visibleMapping.get(math::Vector2{1, 1}), (std::unordered_set<uint32_t>{unitA}));
}

TEST_F(GameTest, damageSystem)
{
	const uint32_t unitA = 1;
	const uint32_t unitB = 2;
	const uint32_t hp = 999;
	const uint32_t damageA = 10;
	const uint32_t damageB = 20;
	context->getDispatcher() << io::SpawnSwordsman{unitA, 1, 1, hp, damageA};
	context->getDispatcher() << io::SpawnSwordsman{unitB, 1, 2, hp, damageB};
	context->getDispatcher().dispatchAll();
	context->advance();

	auto damageTakerA = context->getComponent<DamageTakerComponent>(unitA);
	auto damageTakerB = context->getComponent<DamageTakerComponent>(unitB);

	EXPECT_EQ(damageTakerA->health, hp - damageB);
	EXPECT_EQ(damageTakerB->health, hp - damageA);
}

TEST_F(GameTest, sortView)
{
	const uint32_t unitA = 1;
	const uint32_t unitB = 2;
	context->getDispatcher() << io::SpawnSwordsman{unitA, 1, 1, 1, 1};
	context->getDispatcher() << io::SpawnSwordsman{unitB, 1, 2, 1, 1};
	context->getDispatcher().dispatchAll();

	auto view = context->makeView<BehaviourComponent, MovementComponent>();
	std::sort(
		view.begin(),
		view.end(),
		[](const auto& left, const auto& right)
		{
			return std::get<1>(left)->priority < std::get<1>(right)->priority;
		});

	EXPECT_EQ(std::get<0>(view[0]), unitA);
	EXPECT_EQ(std::get<0>(view[1]), unitB);

	auto behaviourA = context->getComponent<BehaviourComponent>(unitA);
	auto behaviourB = context->getComponent<BehaviourComponent>(unitB);
	behaviourA->priority = behaviourB->priority + 1;

	std::sort(
		view.begin(),
		view.end(),
		[](const auto& left, const auto& right)
		{
			return std::get<1>(left)->priority < std::get<1>(right)->priority;
		});
	EXPECT_EQ(std::get<0>(view[0]), unitB);
	EXPECT_EQ(std::get<0>(view[1]), unitA);
}
