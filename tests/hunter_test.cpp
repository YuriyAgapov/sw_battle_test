#include "game_test_fixture.hpp"

#include <Game/Components/WeaponComponent.hpp>
#include <Game/Events/DamageEvent.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <gtest/gtest.h>

using namespace sw;

TEST_F(GameTest, hunterRanged)
{
	uint32_t hunter = 1;
	uint32_t puppet = 2;
	uint32_t hp = 999;
	uint32_t damage = 1;

	auto dispatcher = context->getDispatcher();
	dispatcher << io::SpawnHunter{hunter, 1, 1, hp, damage, damage, 4};
	dispatcher << io::SpawnSwordsman{puppet, 1, 4, hp, damage};

	std::optional<game::DamageEvent> actualEvent;
	dispatcher.subscribe<game::DamageEvent>(
		[&actualEvent](const game::DamageEvent& event)
		{
			EXPECT_FALSE(actualEvent.has_value());
			actualEvent = event;
		});

	context->getDispatcher().dispatchAll();
	context->advance();

	EXPECT_TRUE(actualEvent.has_value());
	EXPECT_EQ(actualEvent->weaponType, game::WeaponType::Range);
}

TEST_F(GameTest, hunterMelee)
{
	uint32_t hunter = 1;
	uint32_t puppet = 2;
	uint32_t hp = 999;
	uint32_t damage = 1;

	auto dispatcher = context->getDispatcher();
	dispatcher << io::SpawnHunter{hunter, 1, 1, hp, damage, damage, 4};
	dispatcher << io::SpawnSwordsman{puppet, 1, 2, hp, damage};

	std::optional<game::DamageEvent> actualEvent;
	dispatcher.subscribe<game::DamageEvent>(
		[&actualEvent](const game::DamageEvent& event)
		{
			EXPECT_FALSE(actualEvent.has_value());
			actualEvent = event;
		});

	context->getDispatcher().dispatchAll();
	context->advance();

	EXPECT_TRUE(actualEvent.has_value());
	EXPECT_EQ(actualEvent->weaponType, game::WeaponType::Melee);
}
