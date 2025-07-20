#include "game_test_fixture.hpp"

#include <Game/Components/WeaponComponent.hpp>
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
	context->getDispatcher() << io::SpawnHunter{hunter, 1, 1, hp, damage, damage, 4};
	context->getDispatcher() << io::SpawnSwordsman{puppet, 1, 4, hp, damage};
	context->getDispatcher().dispatchAll();
	context->advance();

	auto weaponComponent = context->getComponent<game::WeaponComponent>(hunter);
	EXPECT_EQ(weaponComponent->weapons[0].targetId, puppet);
	EXPECT_EQ(weaponComponent->weapons[1].targetId, InvalidId);
}

TEST_F(GameTest, hunterMelee)
{
	uint32_t hunter = 1;
	uint32_t puppet = 2;
	uint32_t hp = 999;
	uint32_t damage = 1;
	context->getDispatcher() << io::SpawnHunter{hunter, 1, 1, hp, damage, damage, 4};
	context->getDispatcher() << io::SpawnSwordsman{puppet, 1, 4, hp, damage};
	context->getDispatcher().dispatchAll();
	context->advance();

	auto weaponComponent = context->getComponent<game::WeaponComponent>(hunter);
	EXPECT_EQ(weaponComponent->weapons[1].targetId, puppet);
	EXPECT_EQ(weaponComponent->weapons[0].targetId, InvalidId);
}
