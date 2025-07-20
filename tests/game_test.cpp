#include <ECS/Context.hpp>
#include <ECS/System.hpp>
#include <Game/Components/BehaviourComponent.hpp>
#include <Game/Components/DamageTakerComponent.hpp>
#include <Game/Components/GridComponent.hpp>
#include <Game/Components/MovementComponent.hpp>
#include <Game/Components/ViewerComponent.hpp>
#include <Game/Components/VisibleComponent.hpp>
#include <Game/Components/WeaponComponent.hpp>
#include <Game/Events/SetMovementBoundsEvent.hpp>
#include <Game/Events/SetMovementTargetEvent.hpp>
#include <Game/Events/SpawnUnitEvent.hpp>
#include <Game/Systems/AISystem.hpp>
#include <Game/Systems/DamageSystem.hpp>
#include <Game/Systems/MovementSystem.hpp>
#include <Game/Systems/SpawnUnitSystem.hpp>
#include <Game/Systems/VisibilitySystem.hpp>
#include <Game/Systems/WeaponSystem.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <gtest/gtest.h>

using namespace sw;
using namespace sw::ecs;
using namespace sw::game;

class GameTest : public testing::Test
{
protected:
	void SetUp() override
	{
		context = std::make_shared<Context>();

		// init singletone first
		auto grid = context->addSingletoneComponent<GridComponent>();
		grid->bounds = math::Rect2(10, 10);

		// init systems
		context->addSystem(std::make_unique<SpawnUnitSystem>(context));
		context->addSystem(std::make_unique<VisibilitySystem>(context));
		context->addSystem(std::make_unique<AISystem>(context));
		context->addSystem(std::make_unique<WeaponSystem>(context));
		context->addSystem(std::make_unique<DamageSystem>(context));
		context->addSystem(std::make_unique<MovementSystem>(context));
	}

	void TearDown() override
	{
		context.reset();
	}

	std::shared_ptr<Context> context;
};

TEST_F(GameTest, spawnSwordsman)
{
	uint32_t unitId = 1;
	uint32_t hp = 2;
	uint32_t damage = 3;
	context->getDispatcher() << SpawnSwordsmanUnitEvent(io::SpawnSwordsman{unitId, 5, 5, hp, damage});
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
	EXPECT_EQ(weapon->weapons[Weapon::swordId].damage, damage);
	EXPECT_EQ(weapon->weapons[Weapon::swordId].minRange, 0);
	EXPECT_EQ(weapon->weapons[Weapon::swordId].maxRange, 1);
	EXPECT_EQ(weapon->weapons[Weapon::swordId].weaponType, WeaponType::Melee);
	EXPECT_EQ(weapon->weapons[Weapon::swordId].damageType, DamageType::Regular);
	EXPECT_EQ(weapon->weapons[Weapon::swordId].canDamage, std::unordered_set<DispositionType>{DispositionType::Ground});
}

TEST_F(GameTest, movementSystem)
{
	uint32_t unitId = 1;
	uint32_t hp = 2;
	uint32_t damage = 3;
	context->getDispatcher() << SpawnSwordsmanUnitEvent(io::SpawnSwordsman{unitId, 0, 0, hp, damage});
	context->getDispatcher() << SetMovementTargetEvent{unitId, {5, 2}};
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
