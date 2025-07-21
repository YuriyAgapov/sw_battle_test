#include "game_test_fixture.hpp"

#include <Game/Commands/DamageCommand.hpp>
#include <Game/Components/WeaponComponent.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <gtest/gtest.h>

using namespace sw;

TEST_F(GameTest, hunterRanged)
{
	const uint32_t hunter = 1;
	const uint32_t puppet = 2;
	const uint32_t hp = 999;
	const uint32_t damage = 1;

	ecs::EventDispatcher& dispatcher = context->getDispatcher();
	dispatcher << io::SpawnHunter{hunter, 1, 1, hp, damage, damage, 4};
	dispatcher << io::SpawnSwordsman{puppet, 1, 4, hp, damage};

	std::optional<game::DamageCommand> actualCommand;
	dispatcher.subscribe<game::DamageCommand>(
		[&actualCommand](const game::DamageCommand& command)
		{
			if (command.causerId == hunter)
			{
				EXPECT_FALSE(actualCommand.has_value());
				actualCommand = command;
			}
		});

	context->getDispatcher().dispatchAll();
	context->advance();

	EXPECT_TRUE(actualCommand.has_value());
	EXPECT_EQ(actualCommand->weaponType, game::WeaponType::Range);
}

TEST_F(GameTest, hunterMelee)
{
	const uint32_t hunter = 1;
	const uint32_t puppet = 2;
	const uint32_t hp = 999;
	const uint32_t damage = 1;

	ecs::EventDispatcher& dispatcher = context->getDispatcher();
	dispatcher << io::SpawnHunter{hunter, 1, 1, hp, damage, damage, 4};
	dispatcher << io::SpawnSwordsman{puppet, 1, 2, hp, damage};

	std::optional<game::DamageCommand> actualCommand;
	dispatcher.subscribe<game::DamageCommand>(
		[&actualCommand](const game::DamageCommand& command)
		{
			if (command.causerId == hunter)
			{
				EXPECT_FALSE(actualCommand.has_value());
				actualCommand = command;
			}
		});

	context->getDispatcher().dispatchAll();
	context->advance();

	EXPECT_TRUE(actualCommand.has_value());
	EXPECT_EQ(actualCommand->weaponType, game::WeaponType::Melee);
}
